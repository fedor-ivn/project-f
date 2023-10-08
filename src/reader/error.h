#pragma once

#include <iostream>

#include "../ast/span.h"

enum class ErrorCause {
    MissingNumber,
    MissingFractionalPart,
    IntegerOverflow,
    InvalidNumber,
    InvalidIdentifier,
    UnexpectedCharacter,
    UnexpectedRightParenthesis,
    UnclosedList,
};

class SyntaxError : public std::exception {
  public:
    SyntaxError(ErrorCause cause, ast::Span span, bool can_recover);

    ErrorCause cause;
    ast::Span span;
    bool can_recover;

    friend std::ostream&
    operator<<(std::ostream& stream, const SyntaxError& error);
};
