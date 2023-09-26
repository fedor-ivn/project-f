#pragma once

#include <memory>
#include <string_view>

#include "error.h"
#include "span.h"
#include "token.h"

class Scanner {
  public:
    Scanner(std::string_view source, Position offset);

    std::unique_ptr<token::Token> next_token();

  private:
    std::string_view source;
    Position position;

    bool can_peek(size_t at = 0) const;
    char peek(size_t at = 0) const;
    Span advance(size_t by = 1);

    std::unique_ptr<token::Token> parse_symbol();
    std::unique_ptr<token::Token> parse_numeral();
    SyntaxError make_invalid_number_error();
};
