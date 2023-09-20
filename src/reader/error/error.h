enum class ErrorCause {
    MissingIntegerPart
};

class SyntaxError : public std::exception {
public:
    SyntaxError(ErrorCause cause, bool can_recover) {
        this->cause = cause;
        this->can_recover = can_recover;
    }

    ErrorCause cause;
    bool can_recover;
};