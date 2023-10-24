#pragma once

#include <memory>
#include <vector>

#include "../ast/element.h"
#include "scanner.h"

namespace reader {

class Parser {
  public:
    Parser(Scanner scanner);

    std::vector<std::unique_ptr<ast::Element>> parse();

  private:
    Scanner scanner;
    std::unique_ptr<Token> peeked = nullptr;

    std::unique_ptr<Token>& peek_token();
    std::unique_ptr<Token> next_token();
    std::unique_ptr<ast::List> parse_list();
    std::unique_ptr<ast::Element> parse_element();
};

} // namespace reader
