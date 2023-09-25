#include "scanner.h"
#include "error.h"
#include "token.h"
#include <cctype>
#include <iostream>
#include <iterator>
#include <memory>
#include <ostream>
#include <stdexcept>

class ReachedEndOfFile {};

Scanner::Scanner(std::string_view source, Position offset)
    : source(source), position(offset) {}

char Scanner::peek() const {
    if (this->source.empty()) {
        throw ReachedEndOfFile();
    }
    return this->source[0];
}

Span Scanner::advance(size_t by) {
    auto start = this->position;

    by = std::min(by, this->source.size());
    for (size_t past = 0; past < by; ++past) {
        if (this->source[past] == '\n') {
            this->position.to_next_line();
        } else {
            this->position.to_next_column();
        }
    }
    this->source = this->source.substr(by);

    return Span(start, this->position);
}

std::unique_ptr<Token> Scanner::parse_symbol() {
    size_t end = 0;
    while (std::isalpha(this->source[end]) || std::isdigit(this->source[end])) {
        ++end;
    }

    auto symbol = this->source.substr(0, end);
    auto span = this->advance(end);

    if (symbol == "true") {
        return std::make_unique<Boolean>(Boolean(true, span));
    } else if (symbol == "false") {
        return std::make_unique<Boolean>(Boolean(false, span));
    } else if (symbol == "null") {
        return std::make_unique<Null>(Null(span));
    }

    return std::make_unique<Identifier>(Identifier(symbol, span));
}

std::unique_ptr<Token> Scanner::parse_numeral() {
    size_t end = 0;

    if (this->peek() == '+' || this->peek() == '-') {
        ++end;
    }

    if (!std::isdigit(this->peek())) {
        throw SyntaxError(ErrorCause::MissingNumber, true);
    }
    while (std::isdigit(this->source[end])) {
        ++end;
    }

    if (this->source[end] == '.') {
        ++end;
        if (!std::isdigit(this->source[end])) {
            throw SyntaxError(ErrorCause::MissingFractionalPart, true);
        }
        while (std::isdigit(this->source[end])) {
            ++end;
        }

        double real = std::stod(std::string(source.substr(0, end)));
        auto span = this->advance(end);
        return std::make_unique<Real>(Real(real, span));
    }

    try {
        int64_t integer = std::stoll(std::string(source.substr(0, end)));
        auto span = this->advance(end);
        return std::make_unique<Integer>(Integer(integer, span));
    } catch (std::out_of_range) {
        throw SyntaxError(ErrorCause::IntegerOverflow, false);
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
        case '(': {
            auto span = this->advance();
            return std::make_unique<LeftParenthesis>(LeftParenthesis(span));
        }
        case ')': {
            auto span = this->advance();
            return std::make_unique<RightParenthesis>(RightParenthesis(span));
        }
        case '\'': {
            auto span = this->advance();
            return std::make_unique<Apostrophe>(Apostrophe(span));
        }
        }

        if (std::isalpha(character)) {
            return parse_symbol();
        }
        if (std::isdigit(character) || character == '-' || character == '+') {
            return parse_numeral();
        }
    } catch (ReachedEndOfFile) {
        return std::make_unique<EndOfFile>(
            EndOfFile(Span(this->position, this->position)));
    }

    return result;
}
