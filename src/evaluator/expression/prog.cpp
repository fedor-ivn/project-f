#include "../../utils.h"
#include "../error.h"
#include "../expression.h"

namespace evaluator {

using ast::Element;
using ast::List;
using ast::Span;
using utils::Depth;
using utils::to_cons;

Prog::Prog(Span span, Parameters variables, Body body)
    : Expression(span), variables(std::move(variables)), body(std::move(body)) {
}

std::unique_ptr<Prog> Prog::parse(Span span, std::shared_ptr<List> arguments) {
    auto cons = to_cons(arguments);
    if (!cons) {
        throw EvaluationError("`prog` misses a variable list and a body", span);
    }

    auto variable_list = std::dynamic_pointer_cast<List>(cons->left);
    if (!variable_list) {
        throw EvaluationError(
            "`prog` expects a variable list as its first argument",
            cons->left->span
        );
    }
    auto variables = Parameters::parse(variable_list);

    auto body = Body::parse(cons->right);

    return std::make_unique<Prog>(
        Prog(span, std::move(variables), std::move(body))
    );
}

std::shared_ptr<Element> Prog::evaluate(std::shared_ptr<Scope>) const {
    throw std::runtime_error("Not implemented");
}

void Prog::display(std::ostream& stream, size_t depth) const {
    stream << "Prog {\n";

    stream << Depth(depth + 1) << "variables = ";
    this->variables.display(stream, depth + 1);
    stream << '\n';

    stream << Depth(depth + 1) << "body = ";
    this->body.display(stream, depth + 1);
    stream << '\n';

    stream << Depth(depth + 1) << "span = " << this->span << '\n';

    stream << Depth(depth) << '}';
}

bool Prog::returns() const {
    for (auto const& expression : this->body.body) {
        if (expression->returns()) {
            return true;
        }
    }

    return false;
}

bool Prog::breaks() const { return false; }

bool Prog::can_evaluate_to(ast::ElementKind kind) const {
    for (auto const& expression : this->body.body) {
        if (expression->can_break_with(kind)) {
            return true;
        }
        if (expression->diverges()) {
            return false;
        }
    }

    return this->body.body.size() > 0 &&
           this->body.body.back()->can_evaluate_to(kind);
}

bool Prog::can_break_with(ast::ElementKind) const { return false; }
void Prog::validate_no_free_break() const {}
void Prog::validate_no_break_with_value() const {}

} // namespace evaluator
