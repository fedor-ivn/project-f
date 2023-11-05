#include "../../utils.h"
#include "../error.h"
#include "../expression.h"

namespace evaluator {

using ast::Element;
using ast::List;
using ast::Null;
using utils::Depth;

Cond::Cond(
    std::unique_ptr<Expression> condition,
    std::unique_ptr<Expression> then,
    std::unique_ptr<Expression> otherwise
)
    : Expression(), condition(std::move(condition)), then(std::move(then)),
      otherwise(std::move(otherwise)) {}

std::unique_ptr<Cond> Cond::parse(std::shared_ptr<List> arguments) {
    auto cons = arguments->to_cons();

    if (!cons) {
        throw EvaluationError(
            "`cond` needs a condition, a `then` branch, and optionally an "
            "`else` branch",
            arguments->span
        );
    }

    auto condition = Expression::from_element(cons->left);
    if (!condition->can_evaluate_to_boolean()) {
        throw EvaluationError(
            "a boolean is expected, but this expression will never evaluate to "
            "a boolean",
            cons->left->span
        );
    }

    if (!cons->right->to_cons()) {
        throw EvaluationError(
            "`cond` needs at least a `then` branch", cons->span
        );
    }
    cons = cons->right->to_cons();

    auto then = Expression::from_element(cons->left);

    cons = cons->right->to_cons();

    std::unique_ptr<Expression> otherwise;
    if (cons) {
        otherwise = Expression::from_element(cons->left);

        if (cons->right->to_cons()) {
            throw EvaluationError("`cond` has extra arguments", cons->span);
        }
    } else {
        otherwise = Expression::from_element(
            std::make_shared<Null>(Null(arguments->span))
        );
    }

    auto cond =
        Cond(std::move(condition), std::move(then), std::move(otherwise));
    return std::make_unique<Cond>(std::move(cond));
}

std::shared_ptr<Element> Cond::evaluate() const {
    throw std::runtime_error("Not implemented");
}

void Cond::display(std::ostream& stream, size_t depth) const {
    stream << "Cond {\n";

    stream << Depth(depth + 1) << "condition = ";
    this->condition->display(stream, depth + 1);
    stream << '\n';

    stream << Depth(depth + 1) << "then = ";
    this->then->display(stream, depth + 1);
    stream << '\n';

    stream << Depth(depth + 1) << "otherwise = ";
    this->otherwise->display(stream, depth + 1);
    stream << "\n";

    stream << Depth(depth) << '}';
}

bool Cond::can_evaluate_to_function() const {
    return this->then->can_evaluate_to_function() ||
           this->otherwise->can_evaluate_to_function();
}

bool Cond::can_evaluate_to_boolean() const {
    return this->then->can_evaluate_to_boolean() ||
           this->otherwise->can_evaluate_to_boolean();
}

} // namespace evaluator
