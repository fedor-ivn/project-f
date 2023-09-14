#include <iostream>
#include <memory>
#include <ostream>

#include "reader/scanner.h"
#include "reader/token.h"

int main() {
  std::string_view source("(func ; a\nfoo ; b\n(1 ; c\n2.0 -3)\ntrue) ; ; ; ; ...");
  Scanner scanner(source);

  while (true) {
    std::unique_ptr<Token> token = scanner.next_token();
    if (EndOfFile* new_token = dynamic_cast<EndOfFile*>(&*token)) {
      break;
    }
    std::cout << *token;
  }

  return 0;
}
