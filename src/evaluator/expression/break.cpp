#include "../../utils.h"
#include "../control_flow.h"
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

ElementGuard Break::evaluate(EvaluationContext context) const {
    auto element = this->expression->evaluate(context);
    throw BreakControlFlow(std::move(element));
}

void Break::display(std::ostream& stream, size_t depth) const {
    stream << "Break {\n";

    stream << Depth(depth + 1) << "expression = ";
    this->expression->display(stream, depth + 1);
    stream << '\n';

    stream << Depth(depth + 1) << "span = " << this->span << '\n';

    stream << Depth(depth) << '}';
}

bool Break::returns() const { return this->expression->returns(); }
bool Break::breaks() const { return !this->returns(); }
bool Break::can_evaluate_to(ast::ElementKind) const { return false; }

bool Break::can_break_with(ast::ElementKind kind) const {
    return this->expression->can_break_with(kind) ||
           this->expression->can_evaluate_to(kind);
}

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
