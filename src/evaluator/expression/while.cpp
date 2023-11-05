#include "../../utils.h"
#include "../error.h"
#include "../expression.h"

namespace evaluator {

using ast::Element;
using ast::List;
using utils::Depth;
using utils::to_cons;

While::While(std::unique_ptr<Expression> condition, Body body)
    : Expression(), condition(std::move(condition)), body(std::move(body)) {}

std::unique_ptr<While> While::parse(std::shared_ptr<List> arguments) {
    auto form_span = arguments->span;
    auto cons = to_cons(arguments);
    if (!cons) {
        throw EvaluationError("`while` is empty", form_span);
    }

    auto condition = Expression::parse(cons->left);
    if (!condition->can_evaluate_to_boolean()) {
        throw EvaluationError(
            "a boolean is expected, but this expression will never evaluate to "
            "a boolean",
            cons->left->span
        );
    }

    auto body = Body::parse(cons->right);

    return std::make_unique<While>(While(std::move(condition), std::move(body))
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

    stream << Depth(depth) << '}';
}

bool While::can_evaluate_to_function() const { return false; }
bool While::can_evaluate_to_boolean() const { return false; }

} // namespace evaluator
