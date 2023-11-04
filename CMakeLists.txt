cmake_minimum_required(VERSION 3.10)

project(project-f)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

add_library(internals
    src/ast/element.cpp
    src/ast/span.cpp
    src/evaluator/error.cpp
    src/evaluator/evaluator.cpp
    src/evaluator/program.cpp
    src/reader/scanner.cpp
    src/reader/token.cpp
    src/reader/parser.cpp
    src/reader/reader.cpp
    src/reader/error.cpp
    src/utils.cpp
)

add_executable(project-f
    src/main.cpp
)
target_link_libraries(
    project-f PUBLIC internals
)
add_executable(test-runner
    src/test_runner.cpp
)
target_link_libraries(
    test-runner PUBLIC internals
)
