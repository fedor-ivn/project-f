#include "error.h"

namespace evaluator {

using ast::Span;

EvaluationError::EvaluationError(std::string message, Span span)
    : message(message), span(span) {}

std::ostream& operator<<(std::ostream& stream, EvaluationError const& self) {
    stream << "Evaluation error at " << self.span << ": " << self.message;
    return stream;
}

} // namespace evaluator
