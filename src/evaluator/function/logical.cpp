#include "../../ast/kind.h"
#include "../error.h"
#include "../function.h"

namespace evaluator {

using ast::Element;
using ast::ElementKind;

ElementGuard AndFunction::call(CallFrame frame) const {
    if (frame.arguments.size() != 2) {
        throw EvaluationError(
            "`and` expects 2 arguments, received " +
                std::to_string(frame.arguments.size()),
            frame.call_site
        );
    }

    auto a_element = frame.arguments[0];
    auto b_element = frame.arguments[1];

    if (a_element->kind != ElementKind::BOOLEAN ||
        b_element->kind != ElementKind::BOOLEAN) {
        throw EvaluationError(
            "`and` expects arguments to be booleans", frame.call_site
        );
    }

    auto a_bool = std::dynamic_pointer_cast<ast::Boolean>(a_element);
    auto b_bool = std::dynamic_pointer_cast<ast::Boolean>(b_element);
    auto result = a_bool->value && b_bool->value;

    return frame.context.garbage_collector->temporary(
        std::make_shared<ast::Boolean>(result, this->span)
    );
}

std::string_view AndFunction::name() const { return "and"; }

void AndFunction::display_parameters(std::ostream& stream) const {
    stream << "a b";
}

ElementGuard OrFunction::call(CallFrame frame) const {
    if (frame.arguments.size() != 2) {
        throw EvaluationError(
            "`or` expects 2 arguments, received " +
                std::to_string(frame.arguments.size()),
            frame.call_site
        );
    }

    auto a_element = frame.arguments[0];
    auto b_element = frame.arguments[1];

    if (a_element->kind != ElementKind::BOOLEAN ||
        b_element->kind != ElementKind::BOOLEAN) {
        throw EvaluationError(
            "`or` expects arguments to be booleans", frame.call_site
        );
    }

    auto a_bool = std::dynamic_pointer_cast<ast::Boolean>(a_element);
    auto b_bool = std::dynamic_pointer_cast<ast::Boolean>(b_element);
    auto result = a_bool->value || b_bool->value;

    return frame.context.garbage_collector->temporary(
        std::make_shared<ast::Boolean>(result, this->span)
    );
}

std::string_view OrFunction::name() const { return "or"; }

void OrFunction::display_parameters(std::ostream& stream) const {
    stream << "a b";
}

ElementGuard XorFunction::call(CallFrame frame) const {
    if (frame.arguments.size() != 2) {
        throw EvaluationError(
            "`xor` expects 2 arguments, received " +
                std::to_string(frame.arguments.size()),
            frame.call_site
        );
    }

    auto a_element = frame.arguments[0];
    auto b_element = frame.arguments[1];

    if (a_element->kind != ElementKind::BOOLEAN ||
        b_element->kind != ElementKind::BOOLEAN) {
        throw EvaluationError(
            "`xor` expects arguments to be booleans", frame.call_site
        );
    }

    auto a_bool = std::dynamic_pointer_cast<ast::Boolean>(a_element);
    auto b_bool = std::dynamic_pointer_cast<ast::Boolean>(b_element);
    auto result = a_bool->value != b_bool->value;

    return frame.context.garbage_collector->temporary(
        std::make_shared<ast::Boolean>(result, this->span)
    );
}

std::string_view XorFunction::name() const { return "xor"; }

void XorFunction::display_parameters(std::ostream& stream) const {
    stream << "a b";
}

ElementGuard NotFunction::call(CallFrame frame) const {
    if (frame.arguments.size() != 1) {
        throw EvaluationError(
            "`not` expects 1 argument, received " +
                std::to_string(frame.arguments.size()),
            frame.call_site
        );
    }

    auto element = frame.arguments[0];

    if (element->kind != ElementKind::BOOLEAN) {
        throw EvaluationError(
            "`not` expects its argument to be a boolean", frame.call_site
        );
    }

    auto element_bool = std::dynamic_pointer_cast<ast::Boolean>(element);
    auto result = !element_bool->value;

    return frame.context.garbage_collector->temporary(
        std::make_shared<ast::Boolean>(result, this->span)
    );
}

std::string_view NotFunction::name() const { return "not"; }

void NotFunction::display_parameters(std::ostream& stream) const {
    stream << "a";
}

} // namespace evaluator
