#include "../../utils.h"
#include "../error.h"
#include "../expression.h"

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
        Call(form->span, std::move(function), std::move(arguments))
    );
}

std::shared_ptr<Element> Call::evaluate(std::shared_ptr<Scope>) const {
    throw std::runtime_error("Not implemented");
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
