#include "../../ast/kind.h"
#include "../error.h"
#include "../function.h"

namespace evaluator {

using ast::Element;
using ast::ElementKind;

ElementGuard EqualFunction::call(CallFrame frame) const {
    if (frame.arguments.size() != 2) {
        throw EvaluationError(
            "`equal` expects 2 arguments, received " +
                std::to_string(frame.arguments.size()),
            frame.call_site
        );
    }

    auto a_element = frame.arguments[0];
    auto b_element = frame.arguments[1];

    bool result;

    if (a_element->kind == ElementKind::REAL &&
        b_element->kind == ElementKind::REAL) {
        auto a_real = std::dynamic_pointer_cast<ast::Real>(a_element);
        auto b_real = std::dynamic_pointer_cast<ast::Real>(b_element);

        result = a_real->value == b_real->value;
    } else if (a_element->kind == ElementKind::INTEGER && b_element->kind == ElementKind::INTEGER) {
        auto a_int = std::dynamic_pointer_cast<ast::Integer>(a_element);
        auto b_int = std::dynamic_pointer_cast<ast::Integer>(b_element);

        result = a_int->value == b_int->value;
    } else if (a_element->kind == ElementKind::BOOLEAN && b_element->kind == ElementKind::BOOLEAN) {
        auto a_bool = std::dynamic_pointer_cast<ast::Boolean>(a_element);
        auto b_bool = std::dynamic_pointer_cast<ast::Boolean>(b_element);

        result = a_bool->value == b_bool->value;
    } else {
        throw EvaluationError(
            "`equal` expects arguments to be both either integers, reals, or "
            "booleans",
            frame.call_site
        );
    }

    return frame.context.garbage_collector->temporary(
        std::make_shared<ast::Boolean>(result, this->span)
    );
}

std::string_view EqualFunction::name() const { return "equal"; }

void EqualFunction::display_parameters(std::ostream& stream) const {
    stream << "a b";
}

ElementGuard NonequalFunction::call(CallFrame frame) const {
    if (frame.arguments.size() != 2) {
        throw EvaluationError(
            "`nonequal` expects 2 arguments, received " +
                std::to_string(frame.arguments.size()),
            frame.call_site
        );
    }

    auto a_element = frame.arguments[0];
    auto b_element = frame.arguments[1];

    bool result;

    if (a_element->kind == ElementKind::REAL &&
        b_element->kind == ElementKind::REAL) {
        auto a_real = std::dynamic_pointer_cast<ast::Real>(a_element);
        auto b_real = std::dynamic_pointer_cast<ast::Real>(b_element);

        result = a_real->value != b_real->value;
    } else if (a_element->kind == ElementKind::INTEGER && b_element->kind == ElementKind::INTEGER) {
        auto a_int = std::dynamic_pointer_cast<ast::Integer>(a_element);
        auto b_int = std::dynamic_pointer_cast<ast::Integer>(b_element);

        result = a_int->value != b_int->value;
    } else if (a_element->kind == ElementKind::BOOLEAN && b_element->kind == ElementKind::BOOLEAN) {
        auto a_bool = std::dynamic_pointer_cast<ast::Boolean>(a_element);
        auto b_bool = std::dynamic_pointer_cast<ast::Boolean>(b_element);

        result = a_bool->value != b_bool->value;
    } else {
        throw EvaluationError(
            "`nonequal` expects arguments to be both either integers, reals, "
            "or booleans",
            frame.call_site
        );
    }

    return frame.context.garbage_collector->temporary(
        std::make_shared<ast::Boolean>(result, this->span)
    );
}

std::string_view NonequalFunction::name() const { return "nonequal"; }

void NonequalFunction::display_parameters(std::ostream& stream) const {
    stream << "a b";
}

ElementGuard LessFunction::call(CallFrame frame) const {
    if (frame.arguments.size() != 2) {
        throw EvaluationError(
            "`less` expects 2 arguments, received " +
                std::to_string(frame.arguments.size()),
            frame.call_site
        );
    }

    auto a_element = frame.arguments[0];
    auto b_element = frame.arguments[1];

    bool result;

    if (a_element->kind == ElementKind::REAL &&
        b_element->kind == ElementKind::REAL) {
        auto a_real = std::dynamic_pointer_cast<ast::Real>(a_element);
        auto b_real = std::dynamic_pointer_cast<ast::Real>(b_element);

        result = a_real->value < b_real->value;
    } else if (a_element->kind == ElementKind::INTEGER && b_element->kind == ElementKind::INTEGER) {
        auto a_int = std::dynamic_pointer_cast<ast::Integer>(a_element);
        auto b_int = std::dynamic_pointer_cast<ast::Integer>(b_element);

        result = a_int->value < b_int->value;
    } else if (a_element->kind == ElementKind::BOOLEAN && b_element->kind == ElementKind::BOOLEAN) {
        auto a_bool = std::dynamic_pointer_cast<ast::Boolean>(a_element);
        auto b_bool = std::dynamic_pointer_cast<ast::Boolean>(b_element);

        result = a_bool->value < b_bool->value;
    } else {
        throw EvaluationError(
            "`less` expects arguments to be both either integers, reals, "
            "or booleans",
            frame.call_site
        );
    }

    return frame.context.garbage_collector->temporary(
        std::make_shared<ast::Boolean>(result, this->span)
    );
}

std::string_view LessFunction::name() const { return "less"; }

void LessFunction::display_parameters(std::ostream& stream) const {
    stream << "a b";
}

ElementGuard LesseqFunction::call(CallFrame frame) const {
    if (frame.arguments.size() != 2) {
        throw EvaluationError(
            "`lesseq` expects 2 arguments, received " +
                std::to_string(frame.arguments.size()),
            frame.call_site
        );
    }

    auto a_element = frame.arguments[0];
    auto b_element = frame.arguments[1];

    bool result;

    if (a_element->kind == ElementKind::REAL &&
        b_element->kind == ElementKind::REAL) {
        auto a_real = std::dynamic_pointer_cast<ast::Real>(a_element);
        auto b_real = std::dynamic_pointer_cast<ast::Real>(b_element);

        result = a_real->value <= b_real->value;
    } else if (a_element->kind == ElementKind::INTEGER && b_element->kind == ElementKind::INTEGER) {
        auto a_int = std::dynamic_pointer_cast<ast::Integer>(a_element);
        auto b_int = std::dynamic_pointer_cast<ast::Integer>(b_element);

        result = a_int->value <= b_int->value;
    } else if (a_element->kind == ElementKind::BOOLEAN && b_element->kind == ElementKind::BOOLEAN) {
        auto a_bool = std::dynamic_pointer_cast<ast::Boolean>(a_element);
        auto b_bool = std::dynamic_pointer_cast<ast::Boolean>(b_element);

        result = a_bool->value <= b_bool->value;
    } else {
        throw EvaluationError(
            "`lesseq` expects arguments to be both either integers, reals, "
            "or booleans",
            frame.call_site
        );
    }

    return frame.context.garbage_collector->temporary(
        std::make_shared<ast::Boolean>(result, this->span)
    );
}

std::string_view LesseqFunction::name() const { return "lesseq"; }

void LesseqFunction::display_parameters(std::ostream& stream) const {
    stream << "a b";
}

ElementGuard GreaterFunction::call(CallFrame frame) const {
    if (frame.arguments.size() != 2) {
        throw EvaluationError(
            "`greater` expects 2 arguments, received " +
                std::to_string(frame.arguments.size()),
            frame.call_site
        );
    }

    auto a_element = frame.arguments[0];
    auto b_element = frame.arguments[1];

    bool result;

    if (a_element->kind == ElementKind::REAL &&
        b_element->kind == ElementKind::REAL) {
        auto a_real = std::dynamic_pointer_cast<ast::Real>(a_element);
        auto b_real = std::dynamic_pointer_cast<ast::Real>(b_element);

        result = a_real->value > b_real->value;
    } else if (a_element->kind == ElementKind::INTEGER && b_element->kind == ElementKind::INTEGER) {
        auto a_int = std::dynamic_pointer_cast<ast::Integer>(a_element);
        auto b_int = std::dynamic_pointer_cast<ast::Integer>(b_element);

        result = a_int->value > b_int->value;
    } else if (a_element->kind == ElementKind::BOOLEAN && b_element->kind == ElementKind::BOOLEAN) {
        auto a_bool = std::dynamic_pointer_cast<ast::Boolean>(a_element);
        auto b_bool = std::dynamic_pointer_cast<ast::Boolean>(b_element);

        result = a_bool->value > b_bool->value;
    } else {
        throw EvaluationError(
            "`greater` expects arguments to be both either integers, reals, "
            "or booleans",
            frame.call_site
        );
    }

    return frame.context.garbage_collector->temporary(
        std::make_shared<ast::Boolean>(result, this->span)
    );
}

std::string_view GreaterFunction::name() const { return "greater"; }

void GreaterFunction::display_parameters(std::ostream& stream) const {
    stream << "a b";
}

ElementGuard GreatereqFunction::call(CallFrame frame) const {
    if (frame.arguments.size() != 2) {
        throw EvaluationError(
            "`greatereq` expects 2 arguments, received " +
                std::to_string(frame.arguments.size()),
            frame.call_site
        );
    }

    auto a_element = frame.arguments[0];
    auto b_element = frame.arguments[1];

    bool result;

    if (a_element->kind == ElementKind::REAL &&
        b_element->kind == ElementKind::REAL) {
        auto a_real = std::dynamic_pointer_cast<ast::Real>(a_element);
        auto b_real = std::dynamic_pointer_cast<ast::Real>(b_element);

        result = a_real->value >= b_real->value;
    } else if (a_element->kind == ElementKind::INTEGER && b_element->kind == ElementKind::INTEGER) {
        auto a_int = std::dynamic_pointer_cast<ast::Integer>(a_element);
        auto b_int = std::dynamic_pointer_cast<ast::Integer>(b_element);

        result = a_int->value >= b_int->value;
    } else if (a_element->kind == ElementKind::BOOLEAN && b_element->kind == ElementKind::BOOLEAN) {
        auto a_bool = std::dynamic_pointer_cast<ast::Boolean>(a_element);
        auto b_bool = std::dynamic_pointer_cast<ast::Boolean>(b_element);

        result = a_bool->value >= b_bool->value;
    } else {
        throw EvaluationError(
            "`greatereq` expects arguments to be both either integers, reals, "
            "or booleans",
            frame.call_site
        );
    }

    return frame.context.garbage_collector->temporary(
        std::make_shared<ast::Boolean>(result, this->span)
    );
}

std::string_view GreatereqFunction::name() const { return "greatereq"; }

void GreatereqFunction::display_parameters(std::ostream& stream) const {
    stream << "a b";
}

} // namespace evaluator
