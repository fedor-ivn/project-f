#include "../../utils.h"
#include "../error.h"
#include "../expression.h"

namespace evaluator {

using ast::Element;
using ast::List;
using utils::Depth;

Func::Func(std::shared_ptr<ast::Symbol> name, Parameters parameters, Body body)
    : Expression(), name(name), parameters(std::move(parameters)),
      body(std::move(body)) {}

std::unique_ptr<Func> Func::parse(std::shared_ptr<List> arguments) {
    auto cons = arguments->to_cons();
    if (!cons) {
        throw EvaluationError(
            "`func` needs a function name, a parameter list and a body",
            arguments->span
        );
    }

    auto name = std::dynamic_pointer_cast<ast::Symbol>(cons->left);
    if (!name) {
        throw EvaluationError(
            "`func` expected a function name as its first argument",
            cons->left->span
        );
    }

    cons = cons->right->to_cons();
    if (!cons) {
        throw EvaluationError(
            "`func` needs a parameter list and a body", cons->span
        );
    }

    auto parameter_list = std::dynamic_pointer_cast<List>(cons->left);
    if (!parameter_list) {
        throw EvaluationError(
            "`func` expected a parameter list as its second argument",
            cons->left->span
        );
    }
    auto parameters = Parameters::parse(parameter_list);

    cons = cons->right->to_cons();
    if (!cons) {
        throw EvaluationError("`func` needs a body", cons->span);
    }

    std::vector<std::shared_ptr<Element>> elements;
    while (cons) {
        elements.push_back(cons->left);
        cons = cons->right->to_cons();
    }
    auto program = Body::from_elements(elements);

    return std::make_unique<Func>(
        Func(name, std::move(parameters), std::move(program))
    );
}

std::shared_ptr<Element> Func::evaluate() const {
    throw std::runtime_error("Not implemented");
}

void Func::display(std::ostream& stream, size_t depth) const {
    stream << "Func {\n";

    stream << Depth(depth + 1)
           << "name = " << this->name->display_verbose(depth + 1) << '\n';

    stream << Depth(depth + 1) << "arguments = ";
    this->parameters.display(stream, depth + 1);
    stream << '\n';

    stream << Depth(depth + 1) << "body = ";
    this->body.display(stream, depth + 1);
    stream << '\n';

    stream << Depth(depth) << '}';
}

bool Func::can_evaluate_to_function() const { return true; }
bool Func::can_evaluate_to_boolean() const { return false; }

} // namespace evaluator
