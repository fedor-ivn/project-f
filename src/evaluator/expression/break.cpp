#include "../../utils.h"
#include "../error.h"
#include "../expression.h"

namespace evaluator {

using ast::Element;
using ast::List;
using ast::Null;
using ast::Span;
using utils::Depth;
using utils::to_cons;

Break::Break(Span span, std::unique_ptr<Expression> expression)
    : Expression(span), expression(std::move(expression)) {}

std::unique_ptr<Break>
Break::parse(Span span, std::shared_ptr<List> arguments) {
    auto cons = to_cons(arguments);
    if (!cons) {
        auto null = Quote(
            arguments->span, std::make_shared<Null>(Null(arguments->span))
        );
        return std::make_unique<Break>(
            Break(span, std::make_unique<Quote>(null))
        );
    }

    auto expression = Expression::parse(cons->left);

    cons = to_cons(cons->right);
    if (cons) {
        throw EvaluationError("`break` has extra arguments", cons->span);
    }

    return std::make_unique<Break>(Break(span, std::move(expression)));
}

std::shared_ptr<Element> Break::evaluate(std::shared_ptr<Scope>) const {
    throw std::runtime_error("Not implemented");
}

void Break::display(std::ostream& stream, size_t depth) const {
    stream << "Break {\n";

    stream << Depth(depth + 1) << "expression = ";
    this->expression->display(stream, depth + 1);
    stream << '\n';

    stream << Depth(depth + 1) << "span = " << this->span << '\n';

    stream << Depth(depth) << '}';
}

bool Break::can_evaluate_to_function() const { return false; }
bool Break::can_evaluate_to_boolean() const { return false; }

void Break::validate_no_free_break() const {
    throw EvaluationError("`break` outside `while` or `prog`", this->span);
}

void Break::validate_no_break_with_value() const {
    // Detect if the value was explicitly specified
    if (this->expression->span.end != this->span.end) {
        throw EvaluationError(
            "cannot break out of a `while` loop with a value", this->span
        );
    }
}

} // namespace evaluator
