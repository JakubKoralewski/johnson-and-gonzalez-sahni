include_directories(${TEST_SOURCE_DIR}/src ${TEST_SOURCE_DIR}/test)

add_executable(john_test johnsons.c)
add_test(MCN john_test)

add_executable(gonzalez_sahni_test gonzalez_sahni.c)
add_test(MCN gonzalez_sahni_test)

add_executable(input_file input_file.c)
add_test(input_file input_file WORKING_DIRECTORY ${TEST_SOURCE_DIR})
IF (NOT WIN32)
    target_link_libraries(john_test PRIVATE m)
    target_link_libraries(gonzalez_sahni_test PRIVATE m)
    target_link_libraries(input_file PRIVATE m)
ENDIF()
target_compile_definitions(input_file PUBLIC TEST_DIR="${CMAKE_CURRENT_SOURCE_DIR}/")
