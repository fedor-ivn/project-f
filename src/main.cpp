#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <ostream>
#include <sstream>

#include "reader/error.h"
#include "reader/reader.h"
#include "reader/scanner.h"

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
        print_ast(reader);
    }
}

int main(int argc, char** argv) {
    if (argc <= 1) {
        repl();
        return 0;
    }

    for (int nth_file = 1; nth_file < argc; ++nth_file) {
        std::ifstream file(argv[nth_file]);
        std::stringstream buffer;
        buffer << file.rdbuf();

        std::string source(buffer.str());
        Reader reader((std::string_view(source)));

        std::cout << argv[nth_file] << ':' << std::endl;
        print_ast(reader);
    }

    return 0;
}
