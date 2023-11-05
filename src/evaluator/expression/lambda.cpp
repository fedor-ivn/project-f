#include "../../utils.h"
#include "../error.h"
#include "../expression.h"

namespace evaluator {

using ast::Element;
using ast::List;
using utils::Depth;

Lambda::Lambda(Parameters parameters, Body body)
    : Expression(), parameters(std::move(parameters)), body(std::move(body)) {}

std::unique_ptr<Lambda> Lambda::parse(std::shared_ptr<ast::List> arguments) {
    auto cons = arguments->to_cons();
    if (!cons) {
        throw EvaluationError(
            "`lambda` needs a parameter list and a body", arguments->span
        );
    }

    auto parameter_list = std::dynamic_pointer_cast<ast::List>(cons->left);
    if (!parameter_list) {
        throw EvaluationError(
            "`lambda` expected a parameter list as its first argument",
            cons->left->span
        );
    }
    auto parameters = Parameters::parse(parameter_list);

    auto body = Body::parse(cons->right);

    return std::make_unique<Lambda>(
        Lambda(std::move(parameters), std::move(body))
    );
}

std::shared_ptr<ast::Element> Lambda::evaluate() const {
    throw std::runtime_error("Not implemented");
}

void Lambda::display(std::ostream& stream, size_t depth) const {
    stream << "Lambda {\n";

    stream << Depth(depth + 1) << "arguments = ";
    this->parameters.display(stream, depth + 1);
    stream << '\n';

    stream << Depth(depth + 1) << "body = ";
    this->body.display(stream, depth + 1);
    stream << '\n';

    stream << Depth(depth) << '}';
}

bool Lambda::can_evaluate_to_function() const { return true; }
bool Lambda::can_evaluate_to_boolean() const { return false; }

} // namespace evaluator
