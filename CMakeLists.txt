cmake_minimum_required(VERSION 3.22)

project(ccc VERSION 0.0.1 DESCRIPTION "Cimple C Compiler" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_compile_options(-Wall -Wextra -Wpedantic -Werror -Wno-unused-parameter -Wno-deprecated)

add_subdirectory(src)
