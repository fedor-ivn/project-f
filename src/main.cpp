#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

#include "reader/scanner.h"
#include "reader/token.h"

void print_tokens(Scanner& scanner) {
    while (true) {
        std::unique_ptr<Token> token = scanner.next_token();
        if (EndOfFile* new_token = dynamic_cast<EndOfFile*>(&*token)) {
            break;
        }
        std::cout << *token << std::endl;
    }
}

void repl() {
    std::string line;
    size_t nth_line = 0;
    while (std::getline(std::cin, line)) {
        ++nth_line;

        std::string_view source(line);
        Scanner scanner(source, Position(nth_line));
        print_tokens(scanner);
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
        Scanner scanner((std::string_view(source)), Position());

        std::cout << argv[nth_file] << ':' << std::endl;
        print_tokens(scanner);
    }

    return 0;
}
