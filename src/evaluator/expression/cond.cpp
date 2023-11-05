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

Cond::Cond(
    Span span,
    std::unique_ptr<Expression> condition,
    std::unique_ptr<Expression> then,
    std::unique_ptr<Expression> otherwise
)
    : Expression(span), condition(std::move(condition)), then(std::move(then)),
      otherwise(std::move(otherwise)) {}

std::unique_ptr<Cond> Cond::parse(Span span, std::shared_ptr<List> arguments) {
    auto cons = to_cons(arguments);
    if (!cons) {
        throw EvaluationError(
            "`cond` needs a condition, a `then` branch, and optionally an "
            "`else` branch",
            span
        );
    }

    auto condition = Expression::parse(cons->left);
    if (!condition->can_evaluate_to_boolean()) {
        throw EvaluationError(
            "a boolean is expected, but this expression will never evaluate to "
            "a boolean",
            condition->span
        );
    }

    cons = to_cons(cons->right);
    if (!cons) {
        throw EvaluationError("`cond` needs at least a `then` branch", span);
    }
    auto then = Expression::parse(cons->left);

    cons = to_cons(cons->right);
    std::unique_ptr<Expression> otherwise;
    if (cons) {
        otherwise = Expression::parse(cons->left);

        if (to_cons(cons->right)) {
            throw EvaluationError("`cond` has extra arguments", cons->span);
        }
    } else {
        otherwise =
            Expression::parse(std::make_shared<Null>(Null(arguments->span)));
    }

    auto cond =
        Cond(span, std::move(condition), std::move(then), std::move(otherwise));
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

    stream << Depth(depth + 1) << "span = " << this->span << '\n';

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
