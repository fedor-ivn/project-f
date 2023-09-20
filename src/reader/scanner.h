#pragma once

#include <memory>
#include <string_view>

#include "token.h"

class Scanner {
  public:
    Scanner(std::string_view source);

    std::unique_ptr<Token> next_token();

  private:
    std::unique_ptr<Token> parse_symbol();
    std::unique_ptr<Token> parse_numeral();
    std::string_view source;
    size_t current_index = 0;
};
