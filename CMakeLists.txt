cmake_minimum_required(VERSION 3.15)
project(fmi4c C)
set(CMAKE_C_STANDARD 99)

include(CMakeDependentOption)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

option(FMI4C_BUILD_DOCUMENTATION "Build Doxygen documentation" OFF)
option(FMI4C_BUILD_TEST "Build test executable" OFF)
option(FMI4C_BUILD_SHARED "Build as shared library (DLL)" ON)
option(FMI4C_USE_SYSTEM_ZIP "Use system utilities for unzipping" ON)
option(FMI4C_USE_EXTERNAL_MINIZIP "Use minizip target provided by FMI4C_EXTERNAL_MINIZIP" OFF)
#this option is only enabled when FMI4C_USE_SYSTEM_ZIP=OFF
cmake_dependent_option(FMI4C_USE_INCLUDED_ZLIB "Use the included zlib (statically linked) even if a system version is available" OFF "NOT FMI4C_USE_SYSTEM_ZIP" OFF)

if (NOT DEFINED FMI4C_EXTERNAL_MINIZIP)
    set(FMI4C_EXTERNAL_MINIZIP "" CACHE STRING "Defines an external target for minizip.")
endif()

# Set default debug postfix
if (NOT CMAKE_DEBUG_POSTFIX)
    set(CMAKE_DEBUG_POSTFIX d)
endif()

if (FMI4C_BUILD_DOCUMENTATION)
    add_subdirectory(doc)
endif()

if(NOT FMI4C_USE_EXTERNAL_MINIZIP)
    if (NOT FMI4C_USE_SYSTEM_ZIP)
        if(NOT FMI4C_USE_INCLUDED_ZLIB)
            find_package(ZLIB MODULE)
            message(STATUS "ZLIB_FOUND: ${ZLIB_FOUND}")
            message(STATUS "ZLIB_INCLUDE_DIRS: ${ZLIB_INCLUDE_DIRS}")
            message(STATUS "ZLIB_VERSION: ${ZLIB_VERSION} ${ZLIB_VERSION_STRING}")
            message(STATUS "ZLIB_LIBRARIES: ${ZLIB_LIBRARIES}")
        endif()
        # If zlib can not be found in the system, then build local version
        if (NOT ZLIB_FOUND)
            set(zlib_dir "3rdparty/zlib")
            message(STATUS "Building and using included ZLIB: ${zlib_dir}")
            # Must use position independent code if intend to include static zlib into shared (dll) fmi4c lib
            set(CMAKE_POSITION_INDEPENDENT_CODE ON)
            # Add bundled sources, (EXCLUDE_FROM_ALL prevents zlib from being installed with fmi4c)
            add_subdirectory(${zlib_dir} EXCLUDE_FROM_ALL)
            # We highjack the zlibstatic target from the original build system, but it does not have the include directory properties set, so lets add them here
            set_target_properties(zlibstatic
                    PROPERTIES
                    INTERFACE_INCLUDE_DIRECTORIES
                    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${zlib_dir}>;$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/${zlib_dir}>")
            # Use alias to match name that find-module uses
            add_library(ZLIB::ZLIB ALIAS zlibstatic)
        endif()
    endif()
endif()

set(SRCFILES
    src/fmi4c.c
    src/fmi4c_utils.c
    3rdparty/ezxml/ezxml.c
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
    src/fmi4c_private.h)

if(NOT FMI4C_USE_EXTERNAL_MINIZIP)
    if (NOT FMI4C_USE_SYSTEM_ZIP)

        list(APPEND SRCFILES
                3rdparty/minizip/miniunz.c
                3rdparty/minizip/ioapi.c
                3rdparty/minizip/mztools.c
                3rdparty/minizip/unzip.c
        )

        if (WIN32)
            list(APPEND SRCFILES 3rdparty/minizip/iowin32.c)
        endif()

    endif ()
endif()

if (FMI4C_BUILD_SHARED)
    add_library(fmi4c SHARED ${SRCFILES})
else()
    add_library(fmi4c STATIC ${SRCFILES})
endif()

target_compile_definitions(fmi4c PUBLIC HAVE_MEMMOVE=1 EZXML_NOMMAP USE_FILE32API)
if (FMI4C_BUILD_SHARED)
    # Only set DLLEXPORT when producing the library, when consumed dllimport will be assumed
    target_compile_definitions(fmi4c PRIVATE FMI4C_DLLEXPORT)
else()
    # When using as a static library (on windows) FMI4C_STATIC must be defined, otherwise dllimport will be assumed
    target_compile_definitions(fmi4c PUBLIC FMI4C_STATIC)
    # Must use position independent code if intend to include static fmi4c lib into shared (dll) consumer
    set_target_properties(fmi4c PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

target_include_directories(fmi4c
        PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/3rdparty>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

if(NOT FMI4C_USE_EXTERNAL_MINIZIP)
    target_include_directories(fmi4c PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/3rdparty>/minizip")
endif()

# Internal dependency (PRIVATE) on libdl on Linux
target_link_libraries(fmi4c PRIVATE ${CMAKE_DL_LIBS})

if(FMI4C_USE_EXTERNAL_MINIZIP)
    message(STATUS "Using external MINIZIP: ${FMI4C_EXTERNAL_MINIZIP}")
    target_link_libraries(fmi4c PUBLIC ${FMI4C_EXTERNAL_MINIZIP})
    target_compile_definitions(fmi4c PRIVATE FMI4C_WITH_MINIZIP)
else()
    message(STATUS "NOT using external MINIZIP")
    if (NOT FMI4C_USE_SYSTEM_ZIP)
        # Internal dependency (PRIVATE) on zlib
        target_link_libraries(fmi4c PRIVATE ZLIB::ZLIB)
        target_compile_definitions(fmi4c PRIVATE FMI4C_WITH_MINIZIP)
    endif()
endif()

if (FMI4C_BUILD_TEST)
    enable_testing()
    add_subdirectory(test)
    if (WIN32 AND FMI4C_BUILD_SHARED)
        # On Windows there is no RPATH, so fmi4c.dll must be copied to the test directory if test should be runnable from any directory
        add_custom_command(TARGET fmi4c POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:fmi4c> "${CMAKE_CURRENT_BINARY_DIR}/test")
    endif()
endif()


configure_package_config_file(cmake/config.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/fmi4c-config.cmake"
        INSTALL_DESTINATION "${CMAKE_INSTALL_DATADIR}/fmi4c"
        NO_SET_AND_CHECK_MACRO)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/fmi4c-config.cmake"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/fmi4c")

if(NOT FMI4C_USE_EXTERNAL_MINIZIP)
    install(TARGETS fmi4c EXPORT "fmi4c-targets")
endif()

install(DIRECTORY "include/" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
install(DIRECTORY "3rdparty/fmi" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

if(NOT FMI4C_USE_EXTERNAL_MINIZIP)
    # If building a static library, also include the local static zlib if it was built
    if (NOT FMI4C_USE_SYSTEM_ZIP AND NOT FMI4C_BUILD_SHARED AND TARGET zlibstatic)
        install(TARGETS zlibstatic EXPORT "fmi4c-targets")
    endif()

    install(EXPORT "fmi4c-targets"
            NAMESPACE "fmi4c::"
            DESTINATION "${CMAKE_INSTALL_DATADIR}/fmi4c")
endif()
