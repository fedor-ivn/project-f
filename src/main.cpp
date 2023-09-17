#include <iostream>
#include <memory>
#include <ostream>

#include "reader/scanner.h"
#include "reader/token.h"

int main() {
    std::string line;
    while (std::getline(std::cin, line)) {
        std::string_view source(line);
        Scanner scanner(source);

        while (true) {
            std::unique_ptr<Token> token = scanner.next_token();
            if (EndOfFile* new_token = dynamic_cast<EndOfFile*>(&*token)) {
                break;
            }
            std::cout << *token << std::endl;
        }
    }

    return 0;
}
