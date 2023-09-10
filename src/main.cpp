#include <iostream>

#include "reader/scanner.h"

int main() {
  std::string_view source("(func f)");
  Scanner scanner(source);
  scanner.next_token();
  scanner.next_token();
  scanner.next_token();
  scanner.next_token();
  scanner.next_token();
  scanner.next_token();
  scanner.next_token();
  scanner.next_token();
  scanner.next_token();
  return 0;
}
