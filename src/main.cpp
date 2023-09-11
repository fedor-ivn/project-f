#include <iostream>

#include "reader/scanner.h"

int main() {
  std::string_view source("""(func ; a\nfoo ; b\n(1 ; c\n2.0 -3)\ntrue) ; ; ; ; ...""");
  Scanner scanner(source);
  
  for (int source_index = 0; source_index < source.size(); source_index++){
    scanner.next_token();
  }

  return 0;
}
