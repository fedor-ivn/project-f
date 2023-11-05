#include "../../utils.h"
#include "../error.h"
#include "../expression.h"

namespace evaluator {

using ast::Element;
using ast::List;
using utils::Depth;

Prog::Prog(Parameters variables, Program body)
    : Expression(), variables(std::move(variables)), body(std::move(body)) {}

std::unique_ptr<Prog> Prog::parse(std::shared_ptr<List> arguments) {
    auto cons = arguments->to_cons();
    if (!cons) {
        throw EvaluationError(
            "`prog` needs a variable list and a body", arguments->span
        );
    }

    auto variable_list = std::dynamic_pointer_cast<List>(cons->left);
    if (!variable_list) {
        throw EvaluationError(
            "`prog` expected a variable list as its first argument",
            cons->left->span
        );
    }
    auto variables = Parameters::parse(variable_list);

    cons = cons->right->to_cons();
    if (!cons) {
        throw EvaluationError("`prog` needs a body", cons->span);
    }

    std::vector<std::shared_ptr<Element>> elements;
    while (cons) {
        auto element = std::shared_ptr<Element>(cons->left);
        elements.push_back(element);
        cons = cons->right->to_cons();
    }
    auto program = Program::from_elements(elements);

    return std::make_unique<Prog>(Prog(std::move(variables), std::move(program))
    );
}

std::shared_ptr<Element> Prog::evaluate() const {
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

    stream << Depth(depth) << '}';
}

bool Prog::can_evaluate_to_function() const {
    return this->body.can_evaluate_to_function();
}

bool Prog::can_evaluate_to_boolean() const {
    return this->body.can_evaluate_to_boolean();
}

} // namespace evaluator
