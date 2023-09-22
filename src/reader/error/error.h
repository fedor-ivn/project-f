#include <iostream>

enum class ErrorCause {
    MissingIntegerPart
};

class SyntaxError : public std::exception {
public:
    SyntaxError(ErrorCause cause, bool can_recover);

    ErrorCause cause;
    bool can_recover;
};
