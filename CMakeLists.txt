cmake_minimum_required(VERSION 3.0)
project(fmic C)

set(target_name fmic)

add_subdirectory(doc)

include_directories(src)

set(SRCFILES
    src/fmic.c
    src/fmic_utils.c
    include/fmic.h
    include/fmic_public.h
    include/fmic_private.h
    include/fmic_types.h
    include/fmic_types_fmi1.h
    include/fmic_types_fmi2.h
    include/fmic_types_fmi3.h
    include/fmic_functions_fmi1.h
    include/fmic_functions_fmi2.h
    include/fmic_functions_fmi3.h
    include/fmic_utils.h
    3rdparty/minizip/miniunz.c
    3rdparty/minizip/ioapi.c
    3rdparty/minizip/iowin32.c
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
    3rdparty/zlib-1.2.6/trees.c)

add_library(${target_name} SHARED ${SRCFILES})

target_compile_definitions(${target_name} PUBLIC HAVE_MEMMOVE=1)

target_include_directories(${target_name} PUBLIC .)
target_include_directories(${target_name} PUBLIC include)
target_include_directories(${target_name} PUBLIC 3rdparty/minizip)
target_include_directories(${target_name} PUBLIC 3rdparty/zlib-1.2.6)
target_include_directories(${target_name} PUBLIC 3rdparty/libxml2/include)
target_include_directories(${target_name} PUBLIC 3rdparty/libxml2/include/libxml)

set(LIBXML2_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/libxml2/include)

set(LIBXML2_LIBRARY ${CMAKE_SOURCE_DIR}/3rdparty/libxml2/win32/bin.mingw/libxml2.dll)

find_package(LibXml2 REQUIRED)

target_link_libraries(${target_name} ${LIBXML2_LIBRARIES})

install(TARGETS ${target_name} DESTINATION ${CMAKE_INSTALL_PREFIX})
