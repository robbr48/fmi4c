find_package(Threads REQUIRED)

set(libmath "")
if (NOT MSVC)
  set(libmath m)
endif()

set(fmi4ctest_src fmi4c_test.c
                  fmi4c_test_fmi1.c
                  fmi4c_test_fmi2.c
                  fmi4c_test_fmi3.c
                  fmi4c_test.h
                  fmi4c_test_fmi1.h
                  fmi4c_test_fmi2.h
                  fmi4c_test_fmi3.h)
if(NOT MSVC)
  # TODO Implement thread support for MSVC, right now pthreads are expected
  set(fmi4ctest_src ${fmi4ctest_src} fmi4c_test_tlm.c fmi4c_test_tlm.h)
endif()

add_executable(fmi4ctest ${fmi4ctest_src})
target_include_directories(fmi4ctest PRIVATE .)
target_link_libraries(fmi4ctest fmi4c Threads::Threads ${libmath})
install(TARGETS fmi4ctest RUNTIME DESTINATION bin)
if(MSVC)
  target_compile_definitions(fmi4ctest PRIVATE FMI4CTEST_NO_TLM)
endif()

# 3rdparty locations
set(3rdparty ${CMAKE_CURRENT_LIST_DIR}/../3rdparty)
set(sundials ${3rdparty}/sundials)

# Test FMU (FMI 1.0 for co-simulation)
if(WIN32 OR CYGWIN)
  set(binfolder win64)
else()
  set(binfolder linux64)
endif()

add_library(fmi1cs SHARED fmi1cs/fmi1cs.c)
set_target_properties(fmi1cs PROPERTIES PREFIX "")
target_include_directories(fmi1cs PRIVATE ../include/ ${3rdparty}/fmi)
add_custom_command(TARGET fmi1cs POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/fmi1cs/binaries/${binfolder})
add_custom_command(TARGET fmi1cs POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:fmi1cs> ${CMAKE_CURRENT_BINARY_DIR}/fmi1cs/binaries/${binfolder}
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/fmi1cs/modelDescription.xml ${CMAKE_CURRENT_BINARY_DIR}/fmi1cs
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/fmi1cs"
  COMMAND ${CMAKE_COMMAND} -E tar "cvf" "${CMAKE_CURRENT_BINARY_DIR}/fmi1cs.fmu" --format=zip .)
add_test(NAME fmi1cs COMMAND $<TARGET_FILE_NAME:fmi4ctest> -o fmi1cs.out fmi1cs.fmu)

# Test FMU (FMI 1.0 for model exchange)
add_library(fmi1me SHARED fmi1me/fmi1me.c)
set_target_properties(fmi1me PROPERTIES PREFIX "")
target_include_directories(fmi1me PRIVATE ../include/ ${3rdparty}/fmi)
add_custom_command(TARGET fmi1me POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/fmi1me/binaries/${binfolder})
add_custom_command(TARGET fmi1me POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:fmi1me> ${CMAKE_CURRENT_BINARY_DIR}/fmi1me/binaries/${binfolder}
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/fmi1me/modelDescription.xml ${CMAKE_CURRENT_BINARY_DIR}/fmi1me
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/fmi1me"
  COMMAND ${CMAKE_COMMAND} -E tar "cvf" "${CMAKE_CURRENT_BINARY_DIR}/fmi1me.fmu" --format=zip .)
add_test(NAME fmi1me COMMAND $<TARGET_FILE_NAME:fmi4ctest> -o fmi1me.out fmi1me.fmu)

# Test FMU (FMI 2.0 for co-simulation and model exchange)
add_library(fmi2 SHARED fmi2/fmi2.c)
set_target_properties(fmi2 PROPERTIES PREFIX "")
target_include_directories(fmi2 PRIVATE ../include/ ${3rdparty}/fmi)
add_custom_command(TARGET fmi2 POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/fmi2/binaries/${binfolder})
add_custom_command(TARGET fmi2 POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:fmi2> ${CMAKE_CURRENT_BINARY_DIR}/fmi2/binaries/${binfolder}
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/fmi2/modelDescription.xml ${CMAKE_CURRENT_BINARY_DIR}/fmi2/
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/fmi2"
  COMMAND ${CMAKE_COMMAND} -E tar "cvf" "${CMAKE_CURRENT_BINARY_DIR}/fmi2.fmu" --format=zip .)
add_test(NAME fmi2cs COMMAND $<TARGET_FILE_NAME:fmi4ctest> --mode cs -o fmi2.out fmi2.fmu)
add_test(NAME fmi2me COMMAND $<TARGET_FILE_NAME:fmi4ctest> --mode me -o fmi2.out fmi2.fmu)

# Test FMU (FMI 3.0 for co-simulation and model exchange)
if(WIN32 OR CYGWIN)
  set(binfolder x86_64-windows)
else()
  set(binfolder x86_64-linux)
endif()

add_library(fmi3 SHARED fmi3/fmi3.c)
set_target_properties(fmi3 PROPERTIES PREFIX "")
target_include_directories(fmi3 PRIVATE ../include/ ${3rdparty}/fmi)
add_custom_command(TARGET fmi3 POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/fmi3/binaries/${binfolder})
add_custom_command(TARGET fmi3 POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:fmi3> ${CMAKE_CURRENT_BINARY_DIR}/fmi3/binaries/${binfolder}
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/fmi3/modelDescription.xml ${CMAKE_CURRENT_BINARY_DIR}/fmi3
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/fmi3"
  COMMAND ${CMAKE_COMMAND} -E tar "cvf" "${CMAKE_CURRENT_BINARY_DIR}/fmi3.fmu" --format=zip .)
add_test(NAME fmi3cs COMMAND $<TARGET_FILE_NAME:fmi4ctest> --mode cs -o fmi3cs.out fmi3.fmu)
add_test(NAME fmi3me COMMAND $<TARGET_FILE_NAME:fmi4ctest> --mode me -o fmi3me.out fmi3.fmu)

# Test FMU (FMI 3.0 for TLM using intermediate update)
add_library(fmi3tlm SHARED fmi3tlm/fmi3tlm.c
                           ${sundials}/src/nvec_ser/nvector_serial.c
                           ${sundials}/src/sundials/sundials_math.c
                           ${sundials}/include/sundials/sundials_math.h
                           ${sundials}/src/cvode/cvode.c
                           ${sundials}/src/sundials/sundials_nvector.c
                           ${sundials}/src/sundials/sundials_serialization.c
                           ${sundials}/src/cvode/cvode_dense.c
                           ${sundials}/src/cvode/cvode_direct.c
                           ${sundials}/src/cvode/cvode_serialization.c
                           ${sundials}/src/sundials/sundials_direct.c
                           ${sundials}/src/sundials/sundials_dense.c
                           ${sundials}/src/sundials/sundials_timer.c
                           ${sundials}/src/cvode/cvode_io.c)
set_target_properties(fmi3tlm PROPERTIES PREFIX "")
target_include_directories(fmi3tlm PRIVATE ../include/ ${3rdparty}/fmi)
target_include_directories(fmi3tlm PRIVATE ${sundials}/include)
target_link_libraries(fmi3tlm ${libmath})
add_custom_command(TARGET fmi3tlm POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/fmi3tlm/binaries/${binfolder})
add_custom_command(TARGET fmi3tlm POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:fmi3tlm> ${CMAKE_CURRENT_BINARY_DIR}/fmi3tlm/binaries/${binfolder}
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/fmi3tlm/modelDescription.xml ${CMAKE_CURRENT_BINARY_DIR}/fmi3tlm
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/fmi3tlm"
  COMMAND ${CMAKE_COMMAND} -E tar "cvf" "${CMAKE_CURRENT_BINARY_DIR}/fmi3tlm.fmu" --format=zip .)
#if(NOT MSVC)
#  add_test(NAME fmi3tlm COMMAND $<TARGET_FILE_NAME:fmi4ctest> --tlm fmi3tlm.fmu fmi3tlm.fmu fmi3tlm.out)
#endif()

file(COPY ${CMAKE_CURRENT_LIST_DIR}/pytest.py DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_LIST_DIR}/../fmi4c.py DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
