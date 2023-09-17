#include "scanner.h"
#include "token.h"
#include <cctype>
#include <iostream>
#include <iterator>
#include <memory>
#include <ostream>

Scanner::Scanner(std::string_view source) : source(source) {}

std::unique_ptr<Identifier> Scanner::parse_identifier() {
    int begin_index = current_index;

    current_index++;

    while (std::isalpha(source[current_index]) ||
           std::isdigit(source[current_index])) {
        current_index++;
    }

    int end_index = current_index;

    std::string_view identifier =
        source.substr(begin_index, (end_index - begin_index));

    return std::make_unique<Identifier>(Identifier(identifier));
}

std::unique_ptr<Numeral> Scanner::parse_numeral() {
    int begin_index = current_index++;

    while (std::isdigit(source[current_index])) {
        current_index++;
    }

    if (source[current_index] == '.') {
        current_index++;

        while (std::isdigit(source[current_index])) {
            current_index++;
        }

        int end_index = current_index;

        double real = std::stod(
            std::string(source.substr(begin_index, (end_index - begin_index))));

        return std::make_unique<Real>(Real(real));
    } else {
        int end_index = current_index;

        int64_t integer = std::stoll(
            std::string(source.substr(begin_index, (end_index - begin_index))));

        return std::make_unique<Integer>(Integer(integer));
    }
}

std::unique_ptr<Token> Scanner::next_token() {
    std::unique_ptr<Token> result;

    while (std::isspace(source[current_index]) &&
           current_index < source.size()) {
        current_index++;
    }

    while (source[current_index] == ';' && current_index < source.size()) {
        while (source[current_index] != '\n' && current_index < source.size()) {
            current_index++;
        }
        current_index++;
    }

    if (current_index >= source.size()) {
        return std::make_unique<EndOfFile>(EndOfFile());
    }

    switch (source[current_index]) {
    case '(':
        result = std::make_unique<LeftParenthesis>(LeftParenthesis());
        current_index++;
        break;

    case ')':
        result = std::make_unique<RightParenthesis>(RightParenthesis());
        current_index++;
        break;

    default:
        if (std::isalpha(source[current_index])) {
            std::unique_ptr<Identifier> result = parse_identifier();

            if (result->value == "true") {
                std::unique_ptr<Boolean> result =
                    std::make_unique<Boolean>(Boolean(true));
                return result;
            } else if (result->value == "false") {
                std::unique_ptr<Boolean> result =
                    std::make_unique<Boolean>(Boolean(false));
                return result;
            } else if (result->value == "null") {
                std::unique_ptr<Null> result = std::make_unique<Null>(Null());
                return result;
            }

            return result;
        } else if (std::isdigit(source[current_index]) ||
                   source[current_index] == '-' ||
                   source[current_index] == '+') {
            std::unique_ptr<Numeral> result = parse_numeral();
            return result;
        } else if (source[current_index] == '\'') {
            std::unique_ptr<Apostrophe> result =
                std::make_unique<Apostrophe>(Apostrophe());
            current_index++;
            return result;
        }
        break;
    }

    return result;
}
