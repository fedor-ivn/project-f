#include "../../utils.h"
#include "../error.h"
#include "../expression.h"

namespace evaluator {

using ast::Element;
using ast::List;
using ast::Null;
using utils::Depth;

Break::Break(std::unique_ptr<Expression> expression)
    : Expression(), expression(std::move(expression)) {}

std::unique_ptr<Break> Break::parse(std::shared_ptr<List> arguments) {
    auto cons = arguments->to_cons();
    if (!cons) {
        auto null = Quote(std::make_shared<Null>(Null(arguments->span)));
        return std::make_unique<Break>(Break(std::make_unique<Quote>(null)));
    }

    auto expression = Expression::from_element(cons->left);

    cons = cons->right->to_cons();
    if (cons) {
        throw EvaluationError("`break` has extra arguments", cons->span);
    }

    return std::make_unique<Break>(Break(std::move(expression)));
}

std::shared_ptr<Element> Break::evaluate() const {
    throw std::runtime_error("Not implemented");
}

void Break::display(std::ostream& stream, size_t depth) const {
    stream << "Break {\n";

    stream << Depth(depth + 1) << "expression = ";
    this->expression->display(stream, depth + 1);
    stream << '\n';

    stream << Depth(depth) << '}';
}

bool Break::can_evaluate_to_function() const { return false; }
bool Break::can_evaluate_to_boolean() const { return false; }

} // namespace evaluator
