#include "../../utils.h"
#include "../error.h"
#include "../expression.h"
#include "../function.h"

namespace evaluator {

using ast::Cons;
using ast::Element;
using ast::Span;
using utils::Depth;
using utils::to_cons;

Call::Call(
    Span span,
    std::unique_ptr<Expression> function,
    std::vector<std::unique_ptr<Expression>> arguments
)
    : Expression(span), function(std::move(function)),
      arguments(std::move(arguments)) {}

std::unique_ptr<Call> Call::parse(std::shared_ptr<Cons> form) {
    auto function = Expression::parse(form->left);
    if (!function->can_evaluate_to(ast::ElementKind::FUNCTION)) {
        throw EvaluationError(
            "a function (or a special form) is expected, but this expression "
            "will never evaluate to a function",
            function->span
        );
    }

    std::vector<std::unique_ptr<Expression>> arguments;
    auto cons = to_cons(form->right);
    while (cons) {
        arguments.push_back(Expression::parse(cons->left));
        cons = to_cons(cons->right);
    }

    return std::make_unique<Call>(
        form->span, std::move(function), std::move(arguments)
    );
}

ElementGuard Call::evaluate(EvaluationContext context) const {
    auto function_guard = this->function->evaluate(context);
    auto function = std::dynamic_pointer_cast<Function>(*function_guard);
    if (!function) {
        throw EvaluationError("Cannot call a non-function", this->span);
    }

    std::vector<ElementGuard> argument_guards;
    std::vector<std::shared_ptr<Element>> arguments;
    for (auto& argument : this->arguments) {
        auto guard = argument->evaluate(context);
        guard.deactivate();
        arguments.push_back(*guard);
        argument_guards.push_back(std::move(guard));
    }

    CallFrame frame(std::move(arguments), this->span, context);
    return function->call(std::move(frame));
}

void Call::display(std::ostream& stream, size_t depth) const {
    stream << "Call {\n";

    stream << Depth(depth + 1) << "function = ";
    this->function->display(stream, depth + 1);
    stream << '\n';

    stream << Depth(depth + 1) << "arguments = [\n";
    for (auto const& argument : this->arguments) {
        stream << Depth(depth + 2);
        argument->display(stream, depth + 2);
        stream << ",\n";
    }
    stream << Depth(depth + 1) << "]\n";

    stream << Depth(depth + 1) << "span = " << this->span << '\n';

    stream << Depth(depth) << '}';
}

bool Call::returns() const {
    if (this->function->returns()) {
        return true;
    }

    for (auto const& argument : this->arguments) {
        if (argument->returns()) {
            return true;
        }
    }

    return false;
}

bool Call::breaks() const {
    if (this->function->breaks()) {
        return true;
    }

    for (auto const& argument : this->arguments) {
        if (argument->breaks()) {
            return true;
        }
    }

    return false;
}

bool Call::can_evaluate_to(ast::ElementKind) const { return true; }

bool Call::can_break_with(ast::ElementKind kind) const {
    if (this->function->can_break_with(kind)) {
        return true;
    }
    if (this->function->diverges()) {
        return false;
    }

    for (auto const& argument : this->arguments) {
        if (argument->can_break_with(kind)) {
            return true;
        }
        if (argument->diverges()) {
            return false;
        }
    }

    return false;
}

void Call::validate_no_free_break() const {
    this->function->validate_no_free_break();

    for (auto const& argument : this->arguments) {
        argument->validate_no_free_break();
    }
}

void Call::validate_no_break_with_value() const {
    this->function->validate_no_break_with_value();

    for (auto const& argument : this->arguments) {
        argument->validate_no_break_with_value();
    }
}

} // namespace evaluator
