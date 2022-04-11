cmake_minimum_required(VERSION 3.0)
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
target_include_directories(${target_name} PUBLIC 3rdparty)

install(TARGETS ${target_name} DESTINATION ${CMAKE_INSTALL_PREFIX})

if (${BUILD_TEST})
    set(test_target_name fmi4ctest)
    set(TESTSRCFILES
	src/test/fmi4c_test.c
	src/test/fmi4c_test_fmi1.c
	src/test/fmi4c_test_fmi2.c
	src/test/fmi4c_test_fmi3.c
	src/test/fmi4c_test_tlm.c
	include/test/fmi4c_test.h
	include/test/fmi4c_test_fmi1.h
	include/test/fmi4c_test_fmi2.h
	include/test/fmi4c_test_fmi3.h
	include/test/fmi4c_test_tlm.h)
    add_executable(${test_target_name} ${TESTSRCFILES})
    target_include_directories(${test_target_name} PUBLIC include)
    target_include_directories(${test_target_name} PUBLIC include/test)
    install(TARGETS ${test_target_name} DESTINATION ${CMAKE_INSTALL_PREFIX})
    target_link_libraries(${test_target_name} -lm fmi4c)

    #Test FMU (FMI 1.0 for co-simulation)
    set(fmi1cs_target_name fmi1cs)
    set(FMI1CSFILES src/test/fmi1cs/fmi1cs.c)
    add_library(fmi1cs SHARED ${FMI1CSFILES})
    target_include_directories(fmi1cs PUBLIC 3rdparty/fmi)
    if(WIN32)
        set(binfolder win64)
        set(dllext dll)
    else()
        set(binfolder linux64)
        set(dllext so)
    endif()
    add_custom_command(TARGET fmi1cs POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/fmi1cs/binaries/${binfolder})
    add_custom_command(TARGET fmi1cs POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:fmi1cs> ${CMAKE_CURRENT_BINARY_DIR}/fmi1cs/binaries/${binfolder}/fmi1cs.${dllext}
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/src/test/fmi1cs/modelDescription.xml ${CMAKE_CURRENT_BINARY_DIR}/fmi1cs/modelDescription.xml
	WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/fmi1cs"
	COMMAND zip -r "${CMAKE_CURRENT_BINARY_DIR}/fmi1cs.fmu" .)

    #Test FMU (FMI 1.0 for model exchange)
    set(fmi1me_target_name fmi1me)
    set(FMI1MEFILES src/test/fmi1me/fmi1me.c)
    add_library(fmi1me SHARED ${FMI1MEFILES})
    target_include_directories(fmi1me PUBLIC 3rdparty/fmi)
    add_custom_command(TARGET fmi1me POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/fmi1me/binaries/${binfolder})
    add_custom_command(TARGET fmi1me POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:fmi1me> ${CMAKE_CURRENT_BINARY_DIR}/fmi1me/binaries/${binfolder}/fmi1me.${dllext}
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/src/test/fmi1me/modelDescription.xml ${CMAKE_CURRENT_BINARY_DIR}/fmi1me/modelDescription.xml
	WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/fmi1me"
	COMMAND zip -r "${CMAKE_CURRENT_BINARY_DIR}/fmi1me.fmu" .)

    #Test FMU (FMI 2.0 for co-simulation and model exchange)
    set(fmi2_target_name fmi2)
    set(FMI2FILES src/test/fmi2/fmi2.c)
    add_library(fmi2 SHARED ${FMI2FILES})
    target_include_directories(fmi2 PUBLIC 3rdparty/fmi)
    add_custom_command(TARGET fmi2 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/fmi2/binaries/${binfolder})
    add_custom_command(TARGET fmi2 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:fmi2> ${CMAKE_CURRENT_BINARY_DIR}/fmi2/binaries/${binfolder}/fmi2.${dllext}
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/src/test/fmi2/modelDescription.xml ${CMAKE_CURRENT_BINARY_DIR}/fmi2/modelDescription.xml
	WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/fmi2"
	COMMAND zip -r "${CMAKE_CURRENT_BINARY_DIR}/fmi2.fmu" .)

    #Test FMU (FMI 3.0 for co-simulation and model exchange)
    if(WIN32)
        set(binfolder x86_64-windows)
    else()
        set(binfolder x86_64-linux)
    endif()
    set(fmi3_target_name fmi3)
    set(FMI3FILES src/test/fmi3/fmi3.c)
    add_library(fmi3 SHARED ${FMI3FILES})
    target_include_directories(fmi3 PUBLIC 3rdparty/fmi)
    add_custom_command(TARGET fmi3 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/fmi3/binaries/${binfolder})
    add_custom_command(TARGET fmi3 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:fmi3> ${CMAKE_CURRENT_BINARY_DIR}/fmi3/binaries/${binfolder}/fmi3.${dllext}
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/src/test/fmi3/modelDescription.xml ${CMAKE_CURRENT_BINARY_DIR}/fmi3/modelDescription.xml
	WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/fmi3"
	COMMAND zip -r "${CMAKE_CURRENT_BINARY_DIR}/fmi3.fmu" .)

    #Test FMU (FMI 3.0 for TLM using intermediate update)
    set(fmi3tlm_target_name fmi3tlm)
    set(FMI3TLMFILES
	src/test/fmi3tlm/fmi3tlm.c
        3rdparty/sundials/src/nvec_ser/nvector_serial.c
        3rdparty/sundials/src/sundials/sundials_math.c
        3rdparty/sundials/include/sundials/sundials_math.h
        3rdparty/sundials/src/cvode/cvode.c
        3rdparty/sundials/src/sundials/sundials_nvector.c
        3rdparty/sundials/src/sundials/sundials_serialization.c
        3rdparty/sundials/src/cvode/cvode_dense.c
        3rdparty/sundials/src/cvode/cvode_direct.c
        3rdparty/sundials/src/cvode/cvode_serialization.c
        3rdparty/sundials/src/sundials/sundials_direct.c
        3rdparty/sundials/src/sundials/sundials_dense.c
        3rdparty/sundials/src/sundials/sundials_timer.c
        3rdparty/sundials/src/cvode/cvode_io.c)
    add_library(fmi3tlm SHARED ${FMI3TLMFILES})
    target_include_directories(fmi3tlm PUBLIC 3rdparty/fmi)
    target_include_directories(fmi3tlm PUBLIC 3rdparty/sundials/include)
    target_link_libraries(fmi3tlm m)
    add_custom_command(TARGET fmi3tlm POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/fmi3tlm/binaries/${binfolder})
    add_custom_command(TARGET fmi3tlm POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:fmi3tlm> ${CMAKE_CURRENT_BINARY_DIR}/fmi3tlm/binaries/${binfolder}/fmi3tlm.${dllext}
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/src/test/fmi3tlm/modelDescription.xml ${CMAKE_CURRENT_BINARY_DIR}/fmi3tlm/modelDescription.xml
	WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/fmi3tlm"
	COMMAND zip -r "${CMAKE_CURRENT_BINARY_DIR}/fmi3tlm.fmu" .)

endif()
