#include "../../utils.h"
#include "../error.h"
#include "../expression.h"

namespace evaluator {

using ast::Element;
using ast::List;
using ast::Span;
using utils::Depth;
using utils::to_cons;

Lambda::Lambda(Span span, Parameters parameters, Body body)
    : Expression(span), parameters(std::move(parameters)),
      body(std::move(body)) {}

std::unique_ptr<Lambda>
Lambda::parse(Span span, std::shared_ptr<ast::List> arguments) {
    auto cons = to_cons(arguments);
    if (!cons) {
        throw EvaluationError(
            "`lambda` misses a parameter list and a body", span
        );
    }

    auto parameter_list = std::dynamic_pointer_cast<ast::List>(cons->left);
    if (!parameter_list) {
        throw EvaluationError(
            "`lambda` expects a parameter list as its first argument",
            cons->left->span
        );
    }
    auto parameters = Parameters::parse(parameter_list);

    auto body = Body::parse(cons->right);
    body.validate_no_free_break();

    return std::make_unique<Lambda>(
        Lambda(span, std::move(parameters), std::move(body))
    );
}

std::shared_ptr<ast::Element> Lambda::evaluate(std::shared_ptr<Scope>) const {
    throw std::runtime_error("Not implemented");
}

void Lambda::display(std::ostream& stream, size_t depth) const {
    stream << "Lambda {\n";

    stream << Depth(depth + 1) << "parameters = ";
    this->parameters.display(stream, depth + 1);
    stream << '\n';

    stream << Depth(depth + 1) << "body = ";
    this->body.display(stream, depth + 1);
    stream << '\n';

    stream << Depth(depth + 1) << "span = " << this->span << '\n';

    stream << Depth(depth) << '}';
}

bool Lambda::can_evaluate_to_function() const { return true; }
bool Lambda::can_evaluate_to_boolean() const { return false; }
void Lambda::validate_no_free_break() const {}
void Lambda::validate_no_break_with_value() const {}

} // namespace evaluator
