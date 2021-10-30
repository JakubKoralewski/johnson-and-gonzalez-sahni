# CMakeList.txt : CMake project for prg1, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
project(TEST)
add_subdirectory(src)

enable_testing()
add_subdirectory(test)

#project ("imp1")
#
## Add source to this project's executable.
#add_executable (imp1 "src/main.c")
#
## TODO: Add tests and install targets if needed.
#enable_testing()
#add_test(erd test_erd)
