#include <cstdint>
#include <optional>

#include "token.h"

namespace reader {

Token::Token(ast::Span span) : span(span) {}

bool Token::is_left_parenthesis() const {
    return dynamic_cast<LeftParenthesis const*>(this) != nullptr;
}

bool Token::is_right_parenthesis() const {
    return dynamic_cast<RightParenthesis const*>(this) != nullptr;
}

std::optional<std::string_view> Token::to_symbol() const {
    if (auto token = dynamic_cast<Symbol const*>(this)) {
        return std::optional<std::string_view>(token->value);
    }
    return std::nullopt;
}

std::optional<int64_t> Token::to_integer() const {
    if (auto token = dynamic_cast<Integer const*>(this)) {
        return std::optional<int64_t>(token->value);
    }
    return std::nullopt;
}

std::optional<double> Token::to_real() const {
    if (auto token = dynamic_cast<Real const*>(this)) {
        return std::optional<double>(token->value);
    }
    return std::nullopt;
}

std::optional<bool> Token::to_boolean() const {
    if (auto token = dynamic_cast<Boolean const*>(this)) {
        return std::optional<bool>(token->value);
    }
    return std::nullopt;
}

bool Token::is_apostrophe() const {
    return dynamic_cast<Apostrophe const*>(this) != nullptr;
}

bool Token::is_null() const {
    return dynamic_cast<Null const*>(this) != nullptr;
}

bool Token::is_end_of_file() const {
    return dynamic_cast<EndOfFile const*>(this) != nullptr;
}

std::ostream& operator<<(std::ostream& stream, Token const& token) {
    if (token.is_left_parenthesis()) {
        stream << "LeftParenthesis";
    } else if (token.is_right_parenthesis()) {
        stream << "RightParenthesis";
    } else if (auto value = token.to_symbol()) {
        stream << "Symbol(" << *value << ")";
    } else if (auto value = token.to_integer()) {
        stream << "Integer(" << *value << ")";
    } else if (auto value = token.to_real()) {
        stream << "Real(" << *value << ")";
    } else if (auto value = token.to_boolean()) {
        stream << "Boolean(" << *value << ")";
    } else if (token.is_apostrophe()) {
        stream << "Apostrophe";
    } else if (token.is_null()) {
        stream << "Null";
    }

    stream << " at " << token.span;

    return stream;
}

} // namespace reader
