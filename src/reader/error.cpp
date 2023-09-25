#include "error.h"

SyntaxError::SyntaxError(ErrorCause cause, bool can_recover) {
    this->cause = cause;
    this->can_recover = can_recover;
}

std::ostream& operator<<(std::ostream& stream, const SyntaxError& error) {
    stream << "Syntax error: ";

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
    }

    if (error.can_recover) {
        stream << " (can recover)";
    }

    return stream;
}
