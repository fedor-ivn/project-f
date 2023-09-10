#include "scanner.h"

Scanner::Scanner(std::string_view source) : source(source) {}

Token Scanner::next_token() {
    Token result;

    if (current_index >= source.size()) {
        return Null();
    }

    switch (source[current_index])
    {
        case '(':
            std::cout << current_index << " " << "LeftParenthesis" << std::endl;
            result = LeftParenthesis();
            break;
        case ')':
            std::cout << current_index << " " << "RightParenthesis" << std::endl;
            result = RightParenthesis();
            break;
        default:
            break;
    }

    current_index++;

    return result;
}