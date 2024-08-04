cmake_minimum_required(VERSION 3.28)
project(extraclase1_2_0)

set(CMAKE_CXX_STANDARD 17)

add_executable(generator Generator.cpp PagedArray.cpp)
add_executable(sorter sorter.cpp PagedArray.cpp)
