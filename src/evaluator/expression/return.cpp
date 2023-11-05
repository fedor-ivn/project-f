#include "../../utils.h"
#include "../error.h"
#include "../expression.h"

namespace evaluator {

using ast::Element;
using ast::List;
using ast::Null;
using utils::Depth;

Return::Return(std::unique_ptr<Expression> expression)
    : Expression(), expression(std::move(expression)) {}

std::unique_ptr<Return> Return::parse(std::shared_ptr<List> arguments) {
    auto cons = arguments->to_cons();
    if (!cons) {
        auto null = Quote(std::make_shared<Null>(Null(arguments->span)));
        return std::make_unique<Return>(Return(std::make_unique<Quote>(null)));
    }

    auto expression = Expression::from_element(cons->left);

    cons = cons->right->to_cons();
    if (cons) {
        throw EvaluationError("`return` has extra arguments", cons->span);
    }

    return std::make_unique<Return>(Return(std::move(expression)));
}

std::shared_ptr<Element> Return::evaluate() const {
    throw std::runtime_error("Not implemented");
}

void Return::display(std::ostream& stream, size_t depth) const {
    stream << "Return {\n";

    stream << Depth(depth + 1) << "expression = ";
    this->expression->display(stream, depth + 1);
    stream << '\n';

    stream << Depth(depth) << '}';
}

bool Return::can_evaluate_to_function() const { return false; }
bool Return::can_evaluate_to_boolean() const { return false; }

} // namespace evaluator
