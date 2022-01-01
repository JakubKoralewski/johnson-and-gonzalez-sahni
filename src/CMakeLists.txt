add_executable(imp4 main.c)
IF (NOT WIN32)
    target_link_libraries(imp4 PRIVATE m)
ENDIF()
