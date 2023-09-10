#include <iostream>

#include "reader/scanner.h"

int main() {
  std::string_view source("(func f)");
  Scanner scanner(source);
  
  for (int source_index = 0; source_index < source.size(); source_index++){
    scanner.next_token();
  }

  return 0;
}
