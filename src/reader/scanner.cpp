#include "scanner.h"
#include "token.h"
#include <cctype>
#include <iostream>
#include <iterator>
#include <memory>
#include <ostream>

class ReachedEndOfFile {};

Scanner::Scanner(std::string_view source, Position offset)
    : source(source), position(offset) {}

char Scanner::peek() const {
    if (this->source.empty()) {
        throw ReachedEndOfFile();
    }
    return this->source[0];
}

void Scanner::advance(size_t by) {
    by = std::min(by, this->source.size());

    for (size_t past = 0; past < by; ++past) {
        if (this->source[past] == '\n') {
            this->position.to_next_line();
        } else {
            this->position.to_next_column();
        }
    }
    this->source = this->source.substr(by);
}

std::unique_ptr<Token> Scanner::parse_symbol() {
    size_t end = 0;
    while (std::isalpha(this->source[end]) || std::isdigit(this->source[end])) {
        ++end;
    }

    std::string_view symbol = this->source.substr(0, end);
    this->advance(end);

    if (symbol == "true") {
        return std::make_unique<Boolean>(Boolean(true));
    } else if (symbol == "false") {
        return std::make_unique<Boolean>(Boolean(false));
    } else if (symbol == "null") {
        return std::make_unique<Null>(Null());
    }

    return std::make_unique<Identifier>(Identifier(symbol));
}

std::unique_ptr<Token> Scanner::parse_numeral() {
    size_t end = 0;

    if (this->peek() == '+' || this->peek() == '-') {
        ++end;
    }
    while (std::isdigit(this->source[end])) {
        ++end;
    }

    if (this->source[end] == '.') {
        ++end;
        while (std::isdigit(this->source[end])) {
            ++end;
        }

        double real = std::stod(std::string(source.substr(0, end)));
        this->advance(end);
        return std::make_unique<Real>(Real(real));
    } else {
        int64_t integer = std::stoll(std::string(source.substr(0, end)));
        this->advance(end);
        return std::make_unique<Integer>(Integer(integer));
    }
}

std::unique_ptr<Token> Scanner::next_token() {
    std::unique_ptr<Token> result;

    try {
        while (std::isspace(this->peek())) {
            this->advance();
        }

        while (this->peek() == ';') {
            while (this->peek() != '\n') {
                this->advance();
            }
            this->advance();
        }

        char character = this->peek();
        switch (character) {
        case '(':
            this->advance();
            return std::make_unique<LeftParenthesis>(LeftParenthesis());

        case ')':
            this->advance();
            return std::make_unique<RightParenthesis>(RightParenthesis());

        case '\'':
            this->advance();
            return std::make_unique<Apostrophe>(Apostrophe());
        }

        if (std::isalpha(character)) {
            return parse_symbol();
        }
        if (std::isdigit(character) || character == '-' || character == '+') {
            return parse_numeral();
        }
    } catch (ReachedEndOfFile) {
        return std::make_unique<EndOfFile>(EndOfFile());
    }

    return result;
}
