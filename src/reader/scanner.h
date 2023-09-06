#pragma once

#include <string_view>

#include "token.h"

class Scanner {
public:
  Scanner(std::string_view source);

  Token next_token();

private:
  std::string_view source;
};
