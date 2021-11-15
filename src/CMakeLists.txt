add_executable(imp1 main.c)
IF (NOT WIN32)
    target_link_libraries(imp1 PRIVATE m)
ENDIF()
