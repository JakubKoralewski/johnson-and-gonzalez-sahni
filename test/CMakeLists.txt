include_directories(${TEST_SOURCE_DIR}/src ${TEST_SOURCE_DIR}/test)

add_executable(erd_test erd.c)
add_test(ERD erd_test)

add_executable(edd_test edd.c)
add_test(EDD edd_test)

add_executable(spt_test spt.c)
add_test(SPT spt_test)

add_executable(wspt_test wspt.c)
add_test(WSPT wspt_test)

add_executable(input_file input_file.c)
add_test(input_file input_file WORKING_DIRECTORY ${TEST_SOURCE_DIR})
target_compile_definitions(input_file PUBLIC TEST_DIR="${CMAKE_CURRENT_SOURCE_DIR}/")