add_executable(imp3 main.c)
IF (NOT WIN32)
    target_link_libraries(imp3 PRIVATE m)
ENDIF()
