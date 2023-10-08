#pragma once

#include <cstdint>
#include <optional>
#include <ostream>
#include <string_view>

#include "../ast/span.h"

namespace reader {

class Token {
  public:
    ast::Span span;

    Token(ast::Span span);

    virtual ~Token() = default;

    bool is_left_parenthesis() const;
    bool is_right_parenthesis() const;
    std::optional<std::string_view> to_identifier() const;
    std::optional<int64_t> to_integer() const;
    std::optional<double> to_real() const;
    std::optional<bool> to_boolean() const;
    bool is_apostrophe() const;
    bool is_null() const;
    bool is_end_of_file() const;

    friend std::ostream& operator<<(std::ostream& stream, const Token& token);
};

class LeftParenthesis : public Token {
    using Token::Token;
};

class RightParenthesis : public Token {
    using Token::Token;
};

class Identifier : public Token {
  public:
    std::string_view value;

    Identifier(std::string_view value, ast::Span span)
        : Token(span), value(value) {}
};

class Integer : public Token {
  public:
    int64_t value;

    Integer(int64_t value, ast::Span span) : Token(span), value(value) {}
};

class Real : public Token {
  public:
    double value;

    Real(double value, ast::Span span) : Token(span), value(value) {}
};

class Boolean : public Token {
  public:
    bool value;

    Boolean(bool value, ast::Span span) : Token(span), value(value) {}
};

class Apostrophe : public Token {
    using Token::Token;
};

class Null : public Token {
    using Token::Token;
};

class EndOfFile : public Token {
    using Token::Token;
};

} // namespace reader
