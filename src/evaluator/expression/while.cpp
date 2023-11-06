#include "../../utils.h"
#include "../error.h"
#include "../expression.h"

namespace evaluator {

using ast::Element;
using ast::List;
using ast::Span;
using utils::Depth;
using utils::to_cons;

While::While(Span span, std::unique_ptr<Expression> condition, Body body)
    : Expression(span), condition(std::move(condition)), body(std::move(body)) {
}

std::unique_ptr<While>
While::parse(Span span, std::shared_ptr<List> arguments) {
    auto cons = to_cons(arguments);
    if (!cons) {
        throw EvaluationError("`while` is empty", span);
    }

    auto condition = Expression::parse(cons->left);
    if (!condition->can_evaluate_to_boolean()) {
        throw EvaluationError(
            "a boolean is expected, but this expression will never evaluate to "
            "a boolean",
            cons->left->span
        );
    }
    condition->validate_no_break_with_value();

    auto body = Body::parse(cons->right);
    body.validate_no_break_with_value();

    return std::make_unique<While>(
        While(span, std::move(condition), std::move(body))
    );
}

std::shared_ptr<Element> While::evaluate() const {
    throw std::runtime_error("Not implemented");
}

void While::display(std::ostream& stream, size_t depth) const {
    stream << "While {\n";

    stream << Depth(depth + 1) << "condition = ";
    this->condition->display(stream, depth + 1);
    stream << '\n';

    stream << Depth(depth + 1) << "body = ";
    this->body.display(stream, depth + 1);
    stream << '\n';

    stream << Depth(depth + 1) << "span = " << this->span << '\n';

    stream << Depth(depth) << '}';
}

bool While::can_evaluate_to_function() const { return false; }
bool While::can_evaluate_to_boolean() const { return false; }
void While::validate_no_free_break() const {}
void While::validate_no_break_with_value() const {}

} // namespace evaluator
