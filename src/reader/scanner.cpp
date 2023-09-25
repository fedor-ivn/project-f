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

char is_number_end(char character) {
    return character == '(' || character == ')' || character == '\'' ||
           std::isspace(character);
}

bool Scanner::can_peek(size_t at) const { return at < this->source.size(); }
char Scanner::peek(size_t at) const {
    if (!this->can_peek(at)) {
        throw ReachedEndOfFile();
    }
    return this->source[at];
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
    while (this->can_peek(end) && std::isalnum(this->peek(end))) {
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

    if (!this->can_peek(end) || !std::isdigit(this->peek(end))) {
        throw SyntaxError(ErrorCause::MissingNumber, this->advance(end),
                          end == this->source.size());
    }
    while (this->can_peek(end) && std::isdigit(this->peek(end))) {
        ++end;
    }

    if (this->can_peek(end) && this->peek(end) == '.') {
        ++end;

        if (!this->can_peek(end) || !std::isdigit(this->peek(end))) {
            throw SyntaxError(ErrorCause::MissingFractionalPart,
                              this->advance(end), end == this->source.size());
        }
        while (this->can_peek(end) && std::isdigit(this->peek(end))) {
            ++end;
        }
        if (this->can_peek(end) && !is_number_end(this->peek(end))) {
            throw this->make_invalid_number_error();
        }

        double real = std::stod(std::string(source.substr(0, end)));
        auto span = this->advance(end);
        return std::make_unique<Real>(Real(real, span));
    }

    if (this->can_peek(end) && !is_number_end(this->source[end])) {
        throw this->make_invalid_number_error();
    }

    auto literal = this->source.substr(0, end);
    auto span = this->advance(end);
    try {
        int64_t integer = std::stoll(std::string(literal));
        return std::make_unique<Integer>(Integer(integer, span));
    } catch (std::out_of_range) {
        throw SyntaxError(ErrorCause::IntegerOverflow, span, false);
    }
}

SyntaxError Scanner::make_invalid_number_error() {
    auto start = this->position;
    while (this->can_peek() && !is_number_end(this->peek())) {
        this->advance();
    }

    return SyntaxError(ErrorCause::InvalidNumber, Span(start, this->position),
                       false);
}

std::unique_ptr<Token> Scanner::next_token() {
    try {
        while (true) {
            char character = this->peek();
            switch (character) {
            case '(': {
                auto span = this->advance();
                return std::make_unique<LeftParenthesis>(LeftParenthesis(span));
            }
            case ')': {
                auto span = this->advance();
                return std::make_unique<RightParenthesis>(
                    RightParenthesis(span));
            }
            case '\'': {
                auto span = this->advance();
                return std::make_unique<Apostrophe>(Apostrophe(span));
            }
            case ';':
                while (this->peek() != '\n') {
                    this->advance();
                }
                this->advance();
                break;
            }

            if (std::isspace(character)) {
                this->advance();
            }
            if (std::isalpha(character)) {
                return parse_symbol();
            }
            if (std::isdigit(character) || character == '-' ||
                character == '+') {
                return parse_numeral();
            }
        }
    } catch (ReachedEndOfFile) {
    }

    return std::make_unique<EndOfFile>(
        EndOfFile(Span(this->position, this->position)));
}
