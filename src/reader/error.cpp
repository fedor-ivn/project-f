#include "error.h"

namespace reader {

SyntaxError::SyntaxError(ErrorCause cause, ast::Span span, bool can_recover)
    : cause(cause), span(span), can_recover(can_recover) {}

std::ostream& operator<<(std::ostream& stream, const SyntaxError& error) {
    stream << "Syntax error at " << error.span << ": ";

    switch (error.cause) {
    case ErrorCause::MissingNumber:
        stream << "incomplete number literal";
        break;
    case ErrorCause::MissingFractionalPart:
        stream << "real literal misses fractional part";
        break;
    case ErrorCause::IntegerOverflow:
        stream << "integer literal is out of range";
        break;
    case ErrorCause::InvalidNumber:
        stream << "invalid numerical literal";
        break;
    case ErrorCause::InvalidSymbol:
        stream << "invalid symbol";
        break;
    case ErrorCause::UnexpectedCharacter:
        stream << "unexpected character";
        break;
    case ErrorCause::UnexpectedRightParenthesis:
        stream << "unexpected right parenthesis";
        break;
    case ErrorCause::UnclosedList:
        stream << "unclosed list";
        break;
    }

    if (error.can_recover) {
        stream << " (can recover)";
    }

    return stream;
}

} // namespace reader
