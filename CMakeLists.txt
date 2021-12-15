cmake_minimum_required(VERSION 3.0)
project(fmic C)

set(target_name fmic)

option(BUILD_DOCUMENTATION "Build Doxtygen documentation" OFF])
option(BUILD_TEST "Build test executable" OFF])

if (${BUILD_DOCUMENTATION})
    add_subdirectory(doc)
endif()

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
    3rdparty/fmi/fmiFunctions.h
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

if (${BUILD_TEST})
    set(test_target_name fmictest)
    set(TESTSRCFILES
	src/test/fmic_test.c
	src/test/fmic_test_fmi1.c
	src/test/fmic_test_fmi2.c
	src/test/fmic_test_fmi3.c
	src/test/fmic_test_tlm.c
	include/test/fmic_test.h
	include/test/fmic_test_fmi1.h
	include/test/fmic_test_fmi2.h
	include/test/fmic_test_fmi3.h
	include/test/fmic_test_tlm.h)
    add_executable(${test_target_name} ${TESTSRCFILES})
    target_include_directories(${test_target_name} PUBLIC include)
    target_include_directories(${test_target_name} PUBLIC include/test)
    install(TARGETS ${test_target_name} DESTINATION ${CMAKE_INSTALL_PREFIX})
    target_link_libraries(${test_target_name} fmic)

    #Test FMU (FMI 1.0 for co-simulation)
    set(fmi1cs_target_name fmi1cs)
    set(FMI1CSFILES src/test/fmi1cs/fmi1cs.c)
    add_library(fmi1cs SHARED ${FMI1CSFILES})
    target_include_directories(fmi1cs PUBLIC 3rdparty/fmi)
    add_custom_command(TARGET fmi1cs POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/fmi1cs/binaries/win64
	)
    add_custom_command(TARGET fmi1cs POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:fmi1cs> ${CMAKE_CURRENT_BINARY_DIR}/fmi1cs/binaries/win64/fmi1cs.dll
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/src/test/fmi1cs/modelDescription.xml ${CMAKE_CURRENT_BINARY_DIR}/fmi1cs/modelDescription.xml
	WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/fmi1cs"
	COMMAND zip -r "${CMAKE_CURRENT_BINARY_DIR}/fmi1cs.fmu" .)

    #Test FMU (FMI 1.0 for model exchange)
    set(fmi1me_target_name fmi1me)
    set(FMI1MEFILES src/test/fmi1me/fmi1me.c)
    add_library(fmi1me SHARED ${FMI1MEFILES})
    target_include_directories(fmi1me PUBLIC 3rdparty/fmi)
    add_custom_command(TARGET fmi1me POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/fmi1me/binaries/win64)
    add_custom_command(TARGET fmi1me POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:fmi1me> ${CMAKE_CURRENT_BINARY_DIR}/fmi1me/binaries/win64/fmi1me.dll
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/src/test/fmi1me/modelDescription.xml ${CMAKE_CURRENT_BINARY_DIR}/fmi1me/modelDescription.xml
	WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/fmi1me"
	COMMAND zip -r "${CMAKE_CURRENT_BINARY_DIR}/fmi1me.fmu" .)

    #Test FMU (FMI 2.0 for co-simulation and model exchange)
    set(fmi2_target_name fmi2)
    set(FMI2FILES src/test/fmi2/fmi2.c)
    add_library(fmi2 SHARED ${FMI2FILES})
    target_include_directories(fmi2 PUBLIC 3rdparty/fmi)
    add_custom_command(TARGET fmi2 POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/fmi2/binaries/win64)
    add_custom_command(TARGET fmi2 POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:fmi2> ${CMAKE_CURRENT_BINARY_DIR}/fmi2/binaries/win64/fmi2.dll
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/src/test/fmi2/modelDescription.xml ${CMAKE_CURRENT_BINARY_DIR}/fmi2/modelDescription.xml
	WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/fmi2"
	COMMAND zip -r "${CMAKE_CURRENT_BINARY_DIR}/fmi2.fmu" .)

    #Test FMU (FMI 3.0 for co-simulation and model exchange)
    set(fmi3_target_name fmi3)
    set(FMI3FILES src/test/fmi3/fmi3.c)
    add_library(fmi3 SHARED ${FMI3FILES})
    target_include_directories(fmi3 PUBLIC 3rdparty/fmi)
    add_custom_command(TARGET fmi3 POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/fmi3/binaries/x86_64-windows)
    add_custom_command(TARGET fmi3 POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:fmi3> ${CMAKE_CURRENT_BINARY_DIR}/fmi3/binaries/x86_64-windows/fmi3.dll
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/src/test/fmi3/modelDescription.xml ${CMAKE_CURRENT_BINARY_DIR}/fmi3/modelDescription.xml
	WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/fmi3"
	COMMAND zip -r "${CMAKE_CURRENT_BINARY_DIR}/fmi3.fmu" .)
endif()
