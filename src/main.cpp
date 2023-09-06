#include <iostream>

#include "reader/scanner.h"

int main() {
  std::string_view source("(func f)");
  Scanner scanner(source);
  std::cout << "Hello world!" << std::endl;
  return 0;
}
