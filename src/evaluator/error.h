#include <exception>
#include <string>

#include "../ast/span.h"

namespace evaluator {

class EvaluationError : public std::exception {
  public:
    EvaluationError(std::string message, ast::Span span);

    std::string message;
    ast::Span span;

    friend std::ostream&
    operator<<(std::ostream& stream, EvaluationError const& self);
};

} // namespace evaluator
