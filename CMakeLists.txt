cmake_minimum_required(VERSION 3.3)
project(fmi4c C)
set(CMAKE_C_STANDARD 99)

set(target_name fmi4c)

option(FMI4C_BUILD_DOCUMENTATION "Build Doxygen documentation" OFF)
option(FMI4C_BUILD_TEST "Build test executable" OFF)
option(FMI4C_BUILD_SHARED "Build as shared library (DLL)" ON)
option(FMI4C_USE_INCLUDED_ZLIB "Use the included zlib (statically linked) even if a system version is available" OFF)

if (${FMI4C_BUILD_DOCUMENTATION})
    add_subdirectory(doc)
endif()

if(NOT FMI4C_USE_INCLUDED_ZLIB)
    find_package(ZLIB MODULE)
endif()
# If zlib can not be found in the system, then build local version
if (NOT ZLIB_FOUND)
    set(zlib_dir "3rdparty/zlib")
    message(STATUS "Using included ZLIB: ${zlib_dir}")
    # Must use position independent code if intend to include static zlib into shared (dll) fmi4c lib
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    # Add bundled sources, (EXCLUDE_FROM_ALL prevents zlib from being installed with fmi4c)
    add_subdirectory(${zlib_dir} EXCLUDE_FROM_ALL)
    # We highjack the zlibstatic target from the original build system, but it does not have the include directory properties set, so lets add them here
    set_target_properties(zlibstatic PROPERTIES
                                     INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/${zlib_dir};${CMAKE_CURRENT_BINARY_DIR}/${zlib_dir}")
    # Use alias to match name that find-module uses
    add_library(ZLIB::ZLIB ALIAS zlibstatic)
endif()

set(SRCFILES
    src/fmi4c.c
    src/fmi4c_utils.c
    src/fmi4c_private.h
    src/fmi4c_utils.h
    src/fmi4c_placeholders.h
    include/fmi4c.h
    include/fmi4c_public.h
    include/fmi4c_common.h
    include/fmi4c_types.h
    include/fmi4c_types_fmi1.h
    include/fmi4c_types_fmi2.h
    include/fmi4c_types_fmi3.h
    include/fmi4c_functions_fmi1.h
    include/fmi4c_functions_fmi2.h
    include/fmi4c_functions_fmi3.h
    3rdparty/minizip/miniunz.c
    3rdparty/minizip/ioapi.c
    3rdparty/minizip/mztools.c
    3rdparty/minizip/unzip.c
    3rdparty/ezxml/ezxml.c
    3rdparty/ezxml/ezxml.h)
if (WIN32)
    SET(SRCFILES ${SRCFILES} 3rdparty/minizip/iowin32.c)
endif()

if (FMI4C_BUILD_SHARED)
    add_library(${target_name} SHARED ${SRCFILES})
else()
    add_library(${target_name} STATIC ${SRCFILES})
endif()

target_compile_definitions(${target_name} PUBLIC HAVE_MEMMOVE=1 EZXML_NOMMAP USE_FILE32API)
if (FMI4C_BUILD_SHARED)
    # Only set DLLEXPORT when producing the library, when consumed dllimport will be assumed
    target_compile_definitions(${target_name} PRIVATE FMI4C_DLLEXPORT)
else()
    # When using as a static library (on windows) FMI4C_STATIC must be defined, otherwise dllimport will be assumed
    target_compile_definitions(${target_name} PUBLIC FMI4C_STATIC)
    # Must use position independent code if intend to include static fmi4c lib into shared (dll) consumer
    set_target_properties(${target_name} PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

target_include_directories(${target_name} PUBLIC include)
target_include_directories(${target_name} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/3rdparty>)
target_include_directories(${target_name} PRIVATE src)

# Internal dependecy (PRIVATE) on zlib and ${CMAKE_DL_LIBS}) for libdl on Linux
target_link_libraries(${target_name} PRIVATE ZLIB::ZLIB ${CMAKE_DL_LIBS})

install(TARGETS ${target_name}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
install(DIRECTORY include
        DESTINATION .)
install(DIRECTORY 3rdparty/fmi
        DESTINATION include)
# If building a static library, also include the local static zlib if it was built
if (NOT FMI4C_BUILD_SHARED AND TARGET zlibstatic)
    install(TARGETS zlibstatic
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib)
endif()

if (${FMI4C_BUILD_TEST})
  enable_testing()
  add_subdirectory(test)
  if (WIN32 AND FMI4C_BUILD_SHARED)
    # On Windows there is no RPATH, so fmi4c.dll must be copied to the test directory if test should be runnable from any directory
    add_custom_command(TARGET fmi4c POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:fmi4c> ${CMAKE_CURRENT_BINARY_DIR}/test)
  endif()
endif()
