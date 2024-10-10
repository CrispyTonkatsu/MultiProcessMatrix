cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(spvector LANGUAGES C VERSION 0.1)

set(CMAKE_C_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES})
set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})

# Compile Options
add_compile_options(-Wall -Wextra -O2)

# files to compile
add_executable(matrix-parent parent-matrix.c)
add_executable(matrix-child  child-matrix.c)

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/tests" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
