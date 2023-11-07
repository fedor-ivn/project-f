#include "../../utils.h"
#include "../error.h"
#include "../expression.h"

namespace evaluator {

using ast::Element;
using ast::List;
using ast::Span;
using utils::Depth;
using utils::to_cons;

Setq::Setq(
    Span span,
    std::shared_ptr<ast::Symbol> symbol,
    std::unique_ptr<Expression> expression
)
    : Expression(span), variable(symbol), initializer(std::move(expression)) {}

std::unique_ptr<Setq> Setq::parse(Span span, std::shared_ptr<List> arguments) {
    auto cons = to_cons(arguments);
    if (!cons) {
        throw EvaluationError(
            "`setq` misses a variable name and an initializer", span
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
        throw EvaluationError("`setq` misses an initializer", span);
    }

    auto expression = Expression::parse(cons->left);

    cons = to_cons(cons->right);
    if (cons) {
        throw EvaluationError("`setq` has extra arguments", cons->span);
    }

    return std::make_unique<Setq>(Setq(span, symbol, std::move(expression)));
}

std::shared_ptr<Element> Setq::evaluate(std::shared_ptr<Scope> scope) const {
    auto element = this->initializer->evaluate(scope);
    scope->define(*this->variable, element);
    return element;
}

void Setq::display(std::ostream& stream, size_t depth) const {
    stream << "Setq {\n";

    stream << Depth(depth + 1)
           << "variable = " << this->variable->display_verbose(depth + 1)
           << '\n';

    stream << Depth(depth + 1) << "initializer = ";
    this->initializer->display(stream, depth + 1);
    stream << '\n';

    stream << Depth(depth + 1) << "span = " << this->span << '\n';

    stream << Depth(depth) << '}';
}

bool Setq::can_evaluate_to_function() const {
    return this->initializer->can_evaluate_to_function();
}

bool Setq::can_evaluate_to_boolean() const {
    return this->initializer->can_evaluate_to_boolean();
}

void Setq::validate_no_free_break() const {
    this->initializer->validate_no_free_break();
}

void Setq::validate_no_break_with_value() const {
    this->initializer->validate_no_break_with_value();
}

} // namespace evaluator
