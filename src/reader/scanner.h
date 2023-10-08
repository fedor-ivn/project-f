#pragma once

#include <memory>
#include <string_view>

#include "../ast/span.h"
#include "error.h"
#include "token.h"

namespace reader {

class Scanner {
  public:
    Scanner(std::string_view source, ast::Position offset);

    std::unique_ptr<Token> next_token();

  private:
    std::string_view source;
    ast::Position position;

    bool can_peek(size_t at = 0) const;
    char peek(size_t at = 0) const;
    ast::Span advance(size_t by = 1);

    std::unique_ptr<Token> parse_symbol();
    std::unique_ptr<Token> parse_numeral();
    SyntaxError make_literal_error(ErrorCause cause);
};

} // namespace reader
