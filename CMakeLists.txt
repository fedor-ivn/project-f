cmake_minimum_required(VERSION 3.10)

project(project-f)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(project-f
    src/main.cpp
    src/ast/element.cpp
    src/ast/span.cpp
    src/evaluator/evaluator.cpp
    src/evaluator/program.cpp
    src/reader/scanner.cpp
    src/reader/token.cpp
    src/reader/parser.cpp
    src/reader/reader.cpp
    src/reader/error.cpp
)

add_executable(test-runner
    src/test_runner/main.cpp
    src/test_runner/test_runner.cpp
    src/reader/reader.cpp
    src/reader/parser.cpp
    src/reader/error.cpp
    src/reader/token.cpp
    src/ast/element.cpp
    src/ast/span.cpp
    src/reader/scanner.cpp
)