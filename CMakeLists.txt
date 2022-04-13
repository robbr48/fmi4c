cmake_minimum_required(VERSION 3.1)
project(fmi4c C)
set(CMAKE_C_STANDARD 99)

set(target_name fmi4c)

option(BUILD_DOCUMENTATION "Build Doxygen documentation" OFF)
option(BUILD_TEST "Build test executable" OFF)

if (${BUILD_DOCUMENTATION})
    add_subdirectory(doc)
endif()

set(SRCFILES
    src/fmi4c.c
    src/fmi4c_utils.c
    include/fmi4c.h
    include/fmi4c_public.h
    include/fmi4c_private.h
    include/fmi4c_types.h
    include/fmi4c_types_fmi1.h
    include/fmi4c_types_fmi2.h
    include/fmi4c_types_fmi3.h
    include/fmi4c_functions_fmi1.h
    include/fmi4c_functions_fmi2.h
    include/fmi4c_functions_fmi3.h
    include/fmi4c_utils.h
    include/fmi4c_placeholders.h
    3rdparty/minizip/miniunz.c
    3rdparty/minizip/ioapi.c
    3rdparty/minizip/mztools.c
    3rdparty/minizip/unzip.c
    3rdparty/zlib-1.2.6/crc32.c
    3rdparty/zlib-1.2.6/inflate.c
    3rdparty/zlib-1.2.6/adler32.c
    3rdparty/zlib-1.2.6/inftrees.c
    3rdparty/zlib-1.2.6/inffast.c
    3rdparty/zlib-1.2.6/zutil.c
    3rdparty/zlib-1.2.6/gzwrite.c
    3rdparty/zlib-1.2.6/gzlib.c
    3rdparty/zlib-1.2.6/deflate.c
    3rdparty/zlib-1.2.6/trees.c
    3rdparty/ezxml/ezxml.c
    3rdparty/ezxml/ezxml.h)


if (WIN32)
    SET(SRCFILES ${SRCFILES} 3rdparty/minizip/iowin32.c)
endif()

add_library(${target_name} SHARED ${SRCFILES})

target_compile_definitions(${target_name} PUBLIC HAVE_MEMMOVE=1 EZXML_NOMMAP)

target_include_directories(${target_name} PRIVATE src)
target_include_directories(${target_name} PUBLIC include)
target_include_directories(${target_name} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/3rdparty>)

target_link_libraries(${target_name} PRIVATE ${CMAKE_DL_LIBS}) # for libdl on Linux

install(TARGETS ${target_name}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib)
install(DIRECTORY include
        DESTINATION .)
install(DIRECTORY 3rdparty/fmi
        DESTINATION include)

if (${BUILD_TEST})
  enable_testing()
  add_subdirectory(test)
endif()
