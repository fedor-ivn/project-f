#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <ostream>
#include <sstream>

#include "ast/span.h"
#include "evaluator/evaluator.h"
#include "evaluator/program.h"
#include "reader/error.h"
#include "reader/reader.h"
#include "reader/scanner.h"

using ast::Position;
using evaluator::Evaluator;
using evaluator::Program;
using reader::Reader;
using reader::Scanner;
using reader::SyntaxError;

void print_tokens(Scanner& scanner) {
    while (true) {
        try {
            auto token = scanner.next_token();
            if (token->is_end_of_file()) {
                break;
            }
            std::cout << *token << std::endl;
        } catch (SyntaxError error) {
            std::cout << error << std::endl;
            return;
        }
    }
}

void print_ast(Reader& reader) {
    try {
        auto ast = reader.read();
        for (auto&& element : ast) {
            std::cout << *element << std::endl;
        }
    } catch (SyntaxError error) {
        std::cout << error << std::endl;
        return;
    }
}

void repl() {
    Evaluator evaluator;

    std::string line;
    size_t nth_line = 0;
    while (true) {
        std::cout << ">>> ";
        if (!std::getline(std::cin, line)) {
            break;
        }

        ++nth_line;
        std::string_view source(line);
        Reader reader((std::string_view(source)), Position(nth_line));
        auto elements = reader.read();
        auto program = Program::from_elements(std::move(elements));
        auto result = evaluator.evaluate(std::move(program));

        std::cout << *result << std::endl;
    }
}

int main(int argc, char** argv) {
    if (argc > 2) {
        std::cerr << "Error: too many arguments" << std::endl;
        return 1;
    }

    if (argc == 1) {
        repl();
        return 0;
    }

    char* file_name = argv[1];
    std::ifstream file(file_name);
    std::stringstream buffer;
    buffer << file.rdbuf();

    std::string source(buffer.str());
    Reader reader((std::string_view(source)));

    print_ast(reader);

    return 0;
}
