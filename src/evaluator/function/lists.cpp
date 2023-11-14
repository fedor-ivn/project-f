#include "../error.h"
#include "../function.h"

namespace evaluator {

using ast::Cons;
using ast::Element;
using ast::List;

std::shared_ptr<Element> ConsFunction::call(CallFrame frame) const {
    if (frame.arguments.size() != 2) {
        throw EvaluationError(
            "`cons` expects 2 arguments, received " +
                std::to_string(frame.arguments.size()),
            frame.call_site
        );
    }

    auto left = frame.arguments[0];
    auto right = std::dynamic_pointer_cast<List>(frame.arguments[1]);
    if (!right) {
        throw EvaluationError(
            "`cons` expects the second argument to be a list", frame.call_site
        );
    }

    return std::make_unique<Cons>(left, right, frame.call_site);
}

std::string_view ConsFunction::name() const { return "cons"; }
void ConsFunction::display_parameters(std::ostream& stream) const {
    stream << "head tail";
}

} // namespace evaluator
