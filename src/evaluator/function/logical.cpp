#include "../error.h"
#include "../function.h"

namespace evaluator {

using ast::Element;

std::shared_ptr<Element> AndFunction::call(CallFrame frame) const {
    if (frame.arguments.size() != 2) {
        throw EvaluationError(
            "`and` expects 2 arguments, received " +
                std::to_string(frame.arguments.size()),
            frame.call_site
        );
    }

    auto a_element = frame.arguments[0];
    auto b_element = frame.arguments[1];

    if (a_element->kind != b_element->kind) {
        throw EvaluationError(
            "`and` expects arguments to be booleans", frame.call_site
        );
    }

    bool result;

    if (auto a_bool = std::dynamic_pointer_cast<ast::Boolean>(a_element)) {
        auto b_bool = std::dynamic_pointer_cast<ast::Boolean>(b_element);
        result = a_bool->value && b_bool->value;
    } else {
        throw EvaluationError(
            "`and` expects arguments to be booleans", frame.call_site
        );
    }

    return std::make_shared<ast::Boolean>(result, this->span);
}

std::string_view AndFunction::name() const { return "and"; }

void AndFunction::display_parameters(std::ostream& stream) const {
    stream << "a b";
}

} // namespace evaluator
