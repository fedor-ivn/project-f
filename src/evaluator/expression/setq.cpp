#include "../../utils.h"
#include "../error.h"
#include "../expression.h"

namespace evaluator {

using ast::Element;
using ast::List;
using utils::Depth;
using utils::to_cons;

Setq::Setq(
    std::shared_ptr<ast::Symbol> symbol, std::unique_ptr<Expression> expression
)
    : Expression(), variable(symbol), initializer(std::move(expression)) {}

std::unique_ptr<Setq> Setq::parse(std::shared_ptr<List> arguments) {
    auto form_span = arguments->span;
    auto cons = to_cons(arguments);
    if (!cons) {
        throw EvaluationError(
            "`setq` without a variable name and an initializer", form_span
        );
    }

    auto symbol = std::dynamic_pointer_cast<ast::Symbol>(cons->left);
    if (!symbol) {
        throw EvaluationError(
            "`setq` expects a variable name as its first argument",
            cons->left->span
        );
    }

    cons = to_cons(cons->right);
    if (!cons) {
        throw EvaluationError("`setq` without an initializer", form_span);
    }

    auto expression = Expression::parse(cons->left);

    cons = to_cons(cons->right);
    if (cons) {
        throw EvaluationError("`setq` has extra arguments", cons->span);
    }

    return std::make_unique<Setq>(Setq(symbol, std::move(expression)));
}

std::shared_ptr<Element> Setq::evaluate() const {
    throw std::runtime_error("Not implemented");
}

void Setq::display(std::ostream& stream, size_t depth) const {
    stream << "Setq {\n";

    stream << Depth(depth + 1)
           << "variable = " << this->variable->display_verbose(depth + 1)
           << '\n';

    stream << Depth(depth + 1) << "initializer = ";
    this->initializer->display(stream, depth + 1);
    stream << '\n';

    stream << Depth(depth) << '}';
}

bool Setq::can_evaluate_to_function() const {
    return this->initializer->can_evaluate_to_function();
}

bool Setq::can_evaluate_to_boolean() const {
    return this->initializer->can_evaluate_to_boolean();
}

} // namespace evaluator
