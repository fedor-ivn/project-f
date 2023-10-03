cmake_minimum_required(VERSION 3.10)

project(project-f)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)

add_executable(project-f
    src/main.cpp
    src/reader/scanner.cpp
    src/reader/span.cpp
    src/reader/token.cpp
    src/reader/parser.cpp
    src/reader/element.cpp
    src/reader/reader.cpp
    src/reader/error.cpp
)
