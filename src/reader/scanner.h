#pragma once

#include <string_view>
#include <memory>

#include "token.h"

class Scanner {
public:
  std::unique_ptr<Token> next_token();
  std::unique_ptr<Identifier> parse_identifier();
  std::unique_ptr<Numeral> parse_numeral();

  Scanner(std::string_view source);
private:
  std::string_view source;
  size_t current_index = 0;
};
