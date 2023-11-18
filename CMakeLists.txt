cmake_minimum_required(VERSION 3.10)

project(project-f)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

add_library(internals
    src/ast/element.cpp
    src/ast/span.cpp
    src/evaluator/expression/body.cpp
    src/evaluator/expression/break.cpp
    src/evaluator/expression/call.cpp
    src/evaluator/expression/cond.cpp
    src/evaluator/expression/expression.cpp
    src/evaluator/expression/func.cpp
    src/evaluator/expression/lambda.cpp
    src/evaluator/expression/parameters.cpp
    src/evaluator/expression/prog.cpp
    src/evaluator/expression/program.cpp
    src/evaluator/expression/quote.cpp
    src/evaluator/expression/return.cpp
    src/evaluator/expression/setq.cpp
    src/evaluator/expression/symbol.cpp
    src/evaluator/expression/while.cpp
    src/evaluator/error.cpp
    src/evaluator/control_flow.cpp
    src/evaluator/evaluator.cpp
    src/evaluator/function.cpp
    src/evaluator/function/lists.cpp
    src/evaluator/function/evaluation.cpp
    src/evaluator/function/comparisons.cpp
    src/evaluator/scope.cpp
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
