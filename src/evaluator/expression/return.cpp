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

Return::Return(Span span, std::unique_ptr<Expression> expression)
    : Expression(span), expression(std::move(expression)) {}

std::unique_ptr<Return>
Return::parse(Span span, std::shared_ptr<List> arguments) {
    auto cons = to_cons(arguments);
    if (!cons) {
        auto null = Quote(
            arguments->span, std::make_shared<Null>(Null(arguments->span))
        );
        return std::make_unique<Return>(
            Return(span, std::make_unique<Quote>(null))
        );
    }

    auto expression = Expression::parse(cons->left);

    cons = to_cons(cons->right);
    if (cons) {
        throw EvaluationError("`return` has extra arguments", cons->span);
    }

    return std::make_unique<Return>(Return(span, std::move(expression)));
}

std::shared_ptr<Element> Return::evaluate(std::shared_ptr<Scope> scope) const {
    auto element = this->expression->evaluate(scope);
    throw ReturnControlFlow(element);
}

void Return::display(std::ostream& stream, size_t depth) const {
    stream << "Return {\n";

    stream << Depth(depth + 1) << "expression = ";
    this->expression->display(stream, depth + 1);
    stream << '\n';

    stream << Depth(depth + 1) << "span = " << this->span << '\n';

    stream << Depth(depth) << '}';
}

bool Return::returns() const { return !this->breaks(); }
bool Return::breaks() const { return this->expression->breaks(); }
bool Return::can_evaluate_to(ast::ElementKind) const { return false; }

bool Return::can_break_with(ast::ElementKind kind) const {
    return this->expression->can_break_with(kind);
}

void Return::validate_no_free_break() const {
    this->expression->validate_no_free_break();
}

void Return::validate_no_break_with_value() const {
    this->expression->validate_no_break_with_value();
}

} // namespace evaluator
