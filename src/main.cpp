#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <ostream>
#include <sstream>

#include "reader/error.h"
#include "reader/scanner.h"
#include "reader/token.h"

void print_tokens(Scanner& scanner) {
    std::unique_ptr<Token> token;

    while (true) {
        try {
            std::unique_ptr<Token> token = scanner.next_token();
        } catch (SyntaxError e) {
            std::cout << "SyntaxError: ";

            switch (e.cause) {
                case ErrorCause::Integer64Overflow:
                    std::cout << "integer is out of the range";
                    break;
                case ErrorCause::MissingNumber:
                    std::cout << "numeral part is missing";
                    break;
                case ErrorCause::MissingFloatingPart:
                    std::cout << "floating part is missing";
                    break;
                default:
                    std::cout << "Unknown";
            }

            std::cout << std::endl;
            return;
        }
        if (EndOfFile* new_token = dynamic_cast<EndOfFile*>(&*token)) {
            break;
        }
        std::cout << *token << std::endl;
    }
}

void repl() {
    std::string line;
    while (std::getline(std::cin, line)) {
        std::string_view source(line);
        Scanner scanner(source);
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
        Scanner scanner((std::string_view(source)));

        std::cout << argv[nth_file] << ':' << std::endl;
        print_tokens(scanner);
    }

    return 0;
}
