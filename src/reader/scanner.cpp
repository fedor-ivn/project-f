#include "scanner.h"
#include "token.h"
#include <cctype>
#include <cstdint>
#include <iostream>
#include <ostream>
#include <string>
#include <string_view>

Scanner::Scanner(std::string_view source) : source(source) {}

Token Scanner::next_token() {
  Token result;

  if (current_index >= source.size()) {
    return Null();
  }

  switch (source[current_index]) {
  case '(':
    std::cout << "LeftParenthesis" << std::endl;
    result = LeftParenthesis();
    break;
  case ')':
    std::cout << "RightParenthesis" << std::endl;
    result = RightParenthesis();
    break;
  case ';':
    while (source[current_index] != '\n') {
      current_index++;
    }
    break;
  default:
    if (std::isalpha(source[current_index])) {
      int begin_index = current_index;

      current_index++;

      while (std::isalpha(source[current_index]) || std::isdigit(source[current_index])) {
        current_index++;
      }

      int end_index = current_index;

      std::string_view identifier = source.substr(begin_index, (end_index - begin_index));

      if (identifier == "true") {
        std::cout << "True" << std::endl;
        return Boolean(true);
      }

      std::cout << "Identifier `" << identifier << "`" << std::endl;

      return Identifier();
    }
    else if (std::isdigit(source[current_index]) || source[current_index] == '-' || source[current_index] == '+') {
      int begin_index = current_index;

      current_index++;

      while (std::isdigit(source[current_index])) {
        current_index++;
      }

      if (source[current_index] == '.') {
        current_index++;

        while (std::isdigit(source[current_index])) {
          current_index++;
        }

        int end_index = current_index;

        double real = std::stod(std::string(source.substr(begin_index, (end_index - begin_index))));

        std::cout << "Real " << real;

        if (static_cast<int64_t>(real * 10) % 10 == 0) {
          std::cout << ".0";
        }

        std::cout << std::endl;

        return Real(real);
      }
      else {
        int end_index = current_index;

        int64_t integer = std::stoll(std::string(source.substr(begin_index, (end_index - begin_index))));

        std::cout << "Integer " << integer << std::endl;

        return Integer(integer);
      }
    }
    break;
  }

  current_index++;

  return result;
}
