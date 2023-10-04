#pragma once

#include <memory>
#include <vector>

#include "element.h"
#include "scanner.h"

class Parser {
  public:
    Parser(Scanner scanner);

    std::vector<std::unique_ptr<element::Element>> parse();

  private:
    Scanner scanner;
    std::unique_ptr<token::Token> peeked = nullptr;

    std::unique_ptr<token::Token>& peek_token();
    std::unique_ptr<token::Token> next_token();
    std::unique_ptr<element::Element> parse_cons();
    std::unique_ptr<element::Element> parse_element();
};
