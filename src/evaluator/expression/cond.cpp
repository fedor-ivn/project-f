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
            "`cond` misses a condition, a `then` branch, and maybe an `else` "
            "branch",
            span
        );
    }

    auto condition = Expression::parse(cons->left);
    if (!condition->can_evaluate_to(ast::ElementKind::BOOLEAN)) {
        throw EvaluationError(
            "a boolean is expected, but this expression will never evaluate to "
            "a boolean",
            condition->span
        );
    }

    cons = to_cons(cons->right);
    if (!cons) {
        throw EvaluationError(
            "`cond` misses a `then` branch and maybe an `else` branch", span
        );
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

std::shared_ptr<Element> Cond::evaluate(std::shared_ptr<Scope> scope) const {
    auto evaluated_condition = condition->evaluate(scope);

    if (!evaluated_condition) {
        throw EvaluationError("condition cannot be evaluated", condition->span);
    }

    auto boolean_condition = std::dynamic_pointer_cast<ast::Boolean>(evaluated_condition);

    if (!boolean_condition) {
        throw EvaluationError("condition cannot be casted to boolean", evaluated_condition->span);
    }

    if (boolean_condition->value) {
        auto evaluated_then = then->evaluate(scope);

        if (!evaluated_then) {
            throw EvaluationError("`then` case cannot be evaluated", then->span);
        }

        return evaluated_then;
    }
    else {
        auto evaluated_otherwise = otherwise->evaluate(scope);

        if (!evaluated_otherwise) {
            throw EvaluationError("`otherwise` case cannot be evaluated", otherwise->span);
        }

        return evaluated_otherwise;
    }
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

bool Cond::returns() const {
    return this->condition->returns() ||
           (this->then->returns() && this->otherwise->returns());
}

bool Cond::breaks() const {
    return this->condition->breaks() ||
           (this->then->breaks() && this->otherwise->breaks());
}

bool Cond::can_evaluate_to(ast::ElementKind kind) const {
    if (this->condition->diverges()) {
        return false;
    }

    return this->then->can_evaluate_to(kind) ||
           this->otherwise->can_evaluate_to(kind);
}

bool Cond::can_break_with(ast::ElementKind kind) const {
    if (this->condition->can_break_with(kind)) {
        return true;
    }
    if (this->condition->diverges()) {
        return false;
    }

    return this->then->can_break_with(kind) ||
           this->otherwise->can_break_with(kind);
}

void Cond::validate_no_free_break() const {
    this->condition->validate_no_free_break();
    this->then->validate_no_free_break();
    this->otherwise->validate_no_free_break();
}

void Cond::validate_no_break_with_value() const {
    this->condition->validate_no_break_with_value();
    this->then->validate_no_break_with_value();
    this->otherwise->validate_no_break_with_value();
}

} // namespace evaluator
