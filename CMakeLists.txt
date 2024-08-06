cmake_minimum_required(VERSION 3.10)

# Nombre del proyecto
project(MyProject)

# Establece el estándar de C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Añadir ejecutable para Generator
add_executable(generator Generator.cpp PagedArray.cpp)

# Añadir ejecutable para Sorter
add_executable(sorter sorter.cpp PagedArray.cpp)
