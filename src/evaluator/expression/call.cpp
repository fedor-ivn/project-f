#include "../../utils.h"
#include "../error.h"
#include "../expression.h"

namespace evaluator {

using ast::Cons;
using ast::Element;
using utils::Depth;
using utils::to_cons;

Call::Call(
    std::unique_ptr<Expression> function,
    std::vector<std::unique_ptr<Expression>> arguments
)
    : function(std::move(function)), arguments(std::move(arguments)) {}

std::unique_ptr<Call> Call::parse(Cons const& form) {
    auto function = Expression::parse(form.left);
    if (!function->can_evaluate_to_function()) {
        throw EvaluationError(
            "function (or a special form) expected, but this expression will "
            "never evaluate to a function",
            form.left->span
        );
    }

    std::vector<std::unique_ptr<Expression>> arguments;
    auto cons = to_cons(form.right);
    while (cons) {
        arguments.push_back(Expression::parse(cons->left));
        cons = to_cons(cons->right);
    }

    return std::make_unique<Call>(
        Call(std::move(function), std::move(arguments))
    );
}

std::shared_ptr<Element> Call::evaluate() const {
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

    stream << Depth(depth) << '}';
}

bool Call::can_evaluate_to_function() const { return true; }
bool Call::can_evaluate_to_boolean() const { return true; }

} // namespace evaluator
