#include "error.h"

SyntaxError::SyntaxError(ErrorCause cause, bool can_recover) {
    this->cause = cause;
    this->can_recover = can_recover;
}
