cmake_minimum_required(VERSION 3.22)

project(ccc VERSION 0.0.1 DESCRIPTION "Cimple C Compiler" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_compile_options(-Wall -Wextra -Wpedantic -Werror -Wno-unused-parameter)

add_subdirectory(src)
