#include "../error.h"
#include "../function.h"

namespace evaluator {

using ast::Element;

std::shared_ptr<Element> EqualFunction::call(CallFrame frame) const {
    if (frame.arguments.size() != 2) {
        throw EvaluationError(
            "`equal` expects 2 arguments, received " +
                std::to_string(frame.arguments.size()),
            frame.call_site
        );
    }

    auto a_element = frame.arguments[0];
    auto b_element = frame.arguments[1];

    if (a_element->kind != b_element->kind) {
        throw EvaluationError(
            "`equal` expects that arguments have the same type", frame.call_site
        );
    }

    bool result;

    if (auto a_real = std::dynamic_pointer_cast<ast::Real>(a_element)) {
        auto b_real = std::dynamic_pointer_cast<ast::Real>(b_element);
        result = a_real->value == b_real->value;
    } else if (auto a_int = std::dynamic_pointer_cast<ast::Integer>(a_element)) {
        auto b_int = std::dynamic_pointer_cast<ast::Integer>(b_element);
        result = a_int->value == b_int->value;
    } else if (auto a_bool = std::dynamic_pointer_cast<ast::Boolean>(a_element)) {
        auto b_bool = std::dynamic_pointer_cast<ast::Boolean>(b_element);
        result = a_bool->value == b_bool->value;
    } else {
        throw EvaluationError(
            "`equal` expects arguments to be both either integers, reals, or "
            "booleans",
            frame.call_site
        );
    }

    return std::make_shared<ast::Boolean>(result, this->span);
}

std::string_view EqualFunction::name() const { return "equal"; }

void EqualFunction::display_parameters(std::ostream& stream) const {
    stream << "a b";
}

} // namespace evaluator
