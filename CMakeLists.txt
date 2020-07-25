cmake_minimum_required(VERSION 3.16)

project(ECE467 VERSION 0.0.1 DESCRIPTION "UofT ECE 467 Compiler" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

add_subdirectory(src)
