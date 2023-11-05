#include "../../utils.h"
#include "../error.h"
#include "../expression.h"

namespace evaluator {

using ast::Element;
using ast::List;
using utils::Depth;

While::While(std::unique_ptr<Expression> condition, Body body)
    : Expression(), condition(std::move(condition)), body(std::move(body)) {}

std::unique_ptr<While> While::parse(std::shared_ptr<List> arguments) {
    auto form_span = arguments->span;
    auto cons = arguments->to_cons();
    if (!cons) {
        throw EvaluationError("`while` is empty", form_span);
    }

    auto condition = Expression::from_element(cons->left);
    if (!condition->can_evaluate_to_boolean()) {
        throw EvaluationError(
            "a boolean is expected, but this expression will never evaluate to "
            "a boolean",
            cons->left->span
        );
    }

    cons = cons->right->to_cons();
    if (!cons) {
        throw EvaluationError("`while` without a body", form_span);
    }

    std::vector<std::shared_ptr<Element>> elements;
    while (cons) {
        elements.push_back(cons->left);
        cons = cons->right->to_cons();
    }
    auto program = Body::from_elements(elements);

    return std::make_unique<While>(
        While(std::move(condition), std::move(program))
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
