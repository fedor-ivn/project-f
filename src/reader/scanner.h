#pragma once

#include <memory>
#include <string_view>

#include "span.h"
#include "token.h"

class Scanner {
  public:
    Scanner(std::string_view source, Position offset);

    std::unique_ptr<Token> next_token();

  private:
    std::string_view source;
    Position position;

    char peek() const;
    void advance(size_t by = 1);

    std::unique_ptr<Token> parse_symbol();
    std::unique_ptr<Token> parse_numeral();
};
