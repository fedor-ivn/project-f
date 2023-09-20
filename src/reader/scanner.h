#pragma once

#include <memory>
#include <string_view>

#include "token.h"

class Scanner {
  public:
    Scanner(std::string_view source);

    std::unique_ptr<Token> next_token();

  private:
    std::string_view source;

    char peek() const;
    void advance(size_t by = 1);

    std::unique_ptr<Token> parse_symbol();
    std::unique_ptr<Token> parse_numeral();
};
