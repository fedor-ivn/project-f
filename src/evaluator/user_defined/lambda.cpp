#include "../error.h"
#include "../expression.h"
#include "../function.h"

namespace evaluator {

LambdaFunction::LambdaFunction(
    ast::Span span,
    Parameters parameters,
    std::shared_ptr<Body> body,
    std::weak_ptr<Scope> scope
)
    : UserDefinedFunction(span, parameters, body, scope) {}

void LambdaFunction::_display_verbose(std::ostream& stream, size_t) const {
    stream << "LambdaFunction(" << this->span << ")";
}

std::string_view LambdaFunction::name() const { return ""; }

} // namespace evaluator
