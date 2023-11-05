#include "../../utils.h"
#include "../error.h"
#include "../expression.h"

namespace evaluator {

using ast::Element;
using ast::List;
using ast::Span;
using utils::Depth;
using utils::to_cons;

Func::Func(
    Span span,
    std::shared_ptr<ast::Symbol> name,
    Parameters parameters,
    Body body
)
    : Expression(span), name(name), parameters(std::move(parameters)),
      body(std::move(body)) {}

std::unique_ptr<Func> Func::parse(Span span, std::shared_ptr<List> arguments) {
    auto cons = to_cons(arguments);
    if (!cons) {
        throw EvaluationError(
            "`func` needs a function name, a parameter list and a body", span
        );
    }

    auto name = std::dynamic_pointer_cast<ast::Symbol>(cons->left);
    if (!name) {
        throw EvaluationError(
            "`func` expected a function name as its first argument",
            cons->left->span
        );
    }

    cons = to_cons(cons->right);
    if (!cons) {
        throw EvaluationError("`func` needs a parameter list and a body", span);
    }

    auto parameter_list = std::dynamic_pointer_cast<List>(cons->left);
    if (!parameter_list) {
        throw EvaluationError(
            "`func` expected a parameter list as its second argument",
            cons->left->span
        );
    }
    auto parameters = Parameters::parse(parameter_list);

    auto body = Body::parse(cons->right);

    return std::make_unique<Func>(
        Func(span, name, std::move(parameters), std::move(body))
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

    stream << Depth(depth + 1) << "span = " << this->span << '\n';

    stream << Depth(depth) << '}';
}

bool Func::can_evaluate_to_function() const { return true; }
bool Func::can_evaluate_to_boolean() const { return false; }

} // namespace evaluator
