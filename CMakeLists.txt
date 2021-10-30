# CMakeList.txt : CMake project for prg1, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
project(TEST)
add_subdirectory(src)

enable_testing()
add_subdirectory(test)
