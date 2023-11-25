#include "../../ast/kind.h"
#include "../error.h"
#include "../function.h"
#include <functional>
#include <memory>
#include <optional>
#include <string>

namespace evaluator {

using ast::Element;
using ast::ElementKind;

std::optional<double> cast_to_double(std::shared_ptr<Element> element) {
    if (auto integer = std::dynamic_pointer_cast<ast::Integer>(element)) {
        return integer->value;
    } else if (auto real = std::dynamic_pointer_cast<ast::Real>(element)) {
        return real->value;
    } else {
        return std::nullopt;
    }
}

std::shared_ptr<Element> evaluate_arithmetic_operation(
    CallFrame const& frame,
    std::function<int64_t(int64_t, int64_t)> operation_int,
    std::function<double(double, double)> operation_double
) {
    auto a_element = frame.arguments[0];
    auto b_element = frame.arguments[1];
    if (a_element->kind == ElementKind::INTEGER &&
        b_element->kind == ElementKind::INTEGER) {
        auto a_integer = std::dynamic_pointer_cast<ast::Integer>(a_element);
        auto b_integer = std::dynamic_pointer_cast<ast::Integer>(b_element);
        return std::make_shared<ast::Integer>(
            operation_int(a_integer->value, b_integer->value), frame.call_site
        );
    }
    auto a = cast_to_double(a_element);
    auto b = cast_to_double(b_element);

    if (!(a && b)) {
        return nullptr;
    }

    return std::make_shared<ast::Real>(
        operation_double(*a, *b), frame.call_site
    );
}

ElementGuard PlusFunction::call(CallFrame frame) const {
    if (frame.arguments.size() != 2) {
        throw EvaluationError(
            "`plus` expects 2 arguments, received " +
                std::to_string(frame.arguments.size()),
            frame.call_site
        );
    }

    if (auto element = evaluate_arithmetic_operation(
            frame,
            [](int64_t a, int64_t b) { return a + b; },
            [](double a, double b) { return a + b; }
        )) {
        return frame.context.garbage_collector->temporary(element);
    }
    throw EvaluationError(
        "`plus` expects arguments to be either integers or reals",
        frame.call_site
    );
}

std::string_view PlusFunction::name() const { return "plus"; }
void PlusFunction::display_parameters(std::ostream& stream) const {
    stream << "a b";
}

ElementGuard TimesFunction::call(CallFrame frame) const {
    if (frame.arguments.size() != 2) {
        throw EvaluationError(
            "`times` expects 2 arguments, received " +
                std::to_string(frame.arguments.size()),
            frame.call_site
        );
    }

    if (auto element = evaluate_arithmetic_operation(
            frame,
            [](int64_t a, int64_t b) { return a * b; },
            [](double a, double b) { return a * b; }
        )) {
        return frame.context.garbage_collector->temporary(element);
    }
    throw EvaluationError(
        "`times` expects arguments to be either integers or reals",
        frame.call_site
    );
}

std::string_view TimesFunction::name() const { return "times"; }
void TimesFunction::display_parameters(std::ostream& stream) const {
    stream << "a b";
}

ElementGuard MinusFunction::call(CallFrame frame) const {
    if (frame.arguments.size() != 2) {
        throw EvaluationError(
            "`minus` expects 2 arguments, received " +
                std::to_string(frame.arguments.size()),
            frame.call_site
        );
    }

    if (auto element = evaluate_arithmetic_operation(
            frame,
            [](int64_t a, int64_t b) { return a - b; },
            [](double a, double b) { return a - b; }
        )) {
        return frame.context.garbage_collector->temporary(element);
    }
    throw EvaluationError(
        "`minus` expects arguments to be either integers or reals",
        frame.call_site
    );
}

std::string_view MinusFunction::name() const { return "minus"; }
void MinusFunction::display_parameters(std::ostream& stream) const {
    stream << "a b";
}

ElementGuard DivideFunction::call(CallFrame frame) const {
    if (frame.arguments.size() != 2) {
        throw EvaluationError(
            "`divide` expects 2 arguments, received " +
                std::to_string(frame.arguments.size()),
            frame.call_site
        );
    }

    if (auto element = evaluate_arithmetic_operation(
            frame,
            [&frame](int64_t a, int64_t b) {
                if (b == 0) {
                    throw EvaluationError("division by zero", frame.call_site);
                }
                return a / b;
            },
            [&frame](double a, double b) {
                if (b == 0) {
                    throw EvaluationError("division by zero", frame.call_site);
                }
                return a / b;
            }
        )) {
        return frame.context.garbage_collector->temporary(element);
    }
    throw EvaluationError(
        "`divide` expects arguments to be either integers or reals",
        frame.call_site
    );
}

std::string_view DivideFunction::name() const { return "divide"; }
void DivideFunction::display_parameters(std::ostream& stream) const {
    stream << "a b";
}

} // namespace evaluator
