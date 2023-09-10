cmake_minimum_required(VERSION 3.10)

project(project-f)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(project-f
    src/main.cpp
    src/reader/scanner.cpp
    src/reader/token.cpp
)
