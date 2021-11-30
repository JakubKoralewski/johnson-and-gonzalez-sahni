include_directories(${TEST_SOURCE_DIR}/src ${TEST_SOURCE_DIR}/test)

add_executable(ls_test ls.c)
add_test(LS ls_test)

add_executable(lpt_test lpt.c)
add_test(LPT lpt_test)

add_executable(input_file input_file.c)
add_test(input_file input_file WORKING_DIRECTORY ${TEST_SOURCE_DIR})
IF (NOT WIN32)
    target_link_libraries(ls_test PRIVATE m)
    target_link_libraries(lpt_test PRIVATE m)
    target_link_libraries(input_file PRIVATE m)
ENDIF()
target_compile_definitions(input_file PUBLIC TEST_DIR="${CMAKE_CURRENT_SOURCE_DIR}/")
