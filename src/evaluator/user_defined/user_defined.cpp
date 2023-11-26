#include "../error.h"
#include "../function.h"

namespace evaluator {

using ast::Element;

UserDefinedFunction::UserDefinedFunction(
    ast::Span span,
    Parameters parameters,
    std::shared_ptr<Body> body,
    std::shared_ptr<Scope> scope
)
    : Function(span), parameters(parameters), body(body), scope(scope) {}

std::shared_ptr<Element> UserDefinedFunction::call(CallFrame frame) const {
    if (this->parameters.parameters.size() != frame.arguments.size()) {
        throw EvaluationError("TODO", frame.call_site);
    }

    auto scope = std::make_shared<Scope>(this->scope);

    for (size_t parameter_index = 0;
         parameter_index < this->parameters.parameters.size();
         parameter_index++) {
        scope->define(
            *this->parameters.parameters[parameter_index],
            frame.arguments[parameter_index]
        );
    }

    return this->body->evaluate(scope);
}

void UserDefinedFunction::display_parameters(std::ostream& stream) const {
    if (this->parameters.parameters.empty()) {
        return;
    }

    stream << this->parameters.parameters[0]->value;
    for (size_t parameter_index = 1;
         parameter_index < this->parameters.parameters.size();
         parameter_index++) {
        stream << " " << this->parameters.parameters[parameter_index]->value;
    }
}

}; // namespace evaluator
