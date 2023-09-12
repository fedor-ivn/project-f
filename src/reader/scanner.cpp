#include "scanner.h"
#include "token.h"

Scanner::Scanner(std::string_view source) : source(source) {}

std::unique_ptr<Token> Scanner::next_token() {
  std::unique_ptr<Token> result = std::make_unique<Null>(Null());

  if (current_index >= source.size()) {
    return std::make_unique<EndOfFile>(EndOfFile());
  }

  while (source[current_index] == ';') {
    while (source[current_index] != '\n') {
      current_index++;
    }
  }

  switch (source[current_index]) {
  case '(':
    result = std::make_unique<LeftParenthesis>(LeftParenthesis());
    break;

  case ')':
    result = std::make_unique<RightParenthesis>(RightParenthesis());
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
        return std::make_unique<Boolean>(Boolean(true));
      }
      else if (identifier == "false") {
        return std::make_unique<Boolean>(Boolean(false));
      }

      return std::make_unique<Identifier>(Identifier(identifier));
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

        return std::make_unique<Real>(Real(real));
      }
      else {
        int end_index = current_index;

        int64_t integer = std::stoll(std::string(source.substr(begin_index, (end_index - begin_index))));

        return std::make_unique<Integer>(Integer(integer));
      }
    }
    break;
  }

  current_index++;

  return result;
}
