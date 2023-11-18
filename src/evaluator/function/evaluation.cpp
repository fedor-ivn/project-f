#include "../error.h"
#include "../expression.h"
#include "../function.h"

namespace evaluator {

using ast::Element;

std::shared_ptr<Element> EvalFunction::call(CallFrame frame) const {
    if (frame.arguments.size() != 1) {
        throw EvaluationError(
            "`eval` expects 1 argument, received " +
                std::to_string(frame.arguments.size()),
            frame.call_site
        );
    }

    auto expression = Expression::parse(frame.arguments[0]);
    return expression->evaluate(frame.caller_scope);
}

std::string_view EvalFunction::name() const { return "eval"; }
void EvalFunction::display_parameters(std::ostream& stream) const {
    stream << "program";
}

} // namespace evaluator
