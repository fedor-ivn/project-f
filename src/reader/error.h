#include <iostream>

#pragma once

enum class ErrorCause {
    MissingNumber,
    Integer64Overflow,
    MissingFloatingPart,
    MissingIntegerPart,
};

class SyntaxError : public std::exception {
public:
    SyntaxError(ErrorCause cause, bool can_recover);

    ErrorCause cause;
    bool can_recover;
};
