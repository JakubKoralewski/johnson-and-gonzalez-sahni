add_executable(imp2 main.c)
IF (NOT WIN32)
    target_link_libraries(imp2 PRIVATE m)
ENDIF()
