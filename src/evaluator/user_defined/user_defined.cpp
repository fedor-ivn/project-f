#include "../control_flow.h"
#include "../error.h"
#include "../function.h"

namespace evaluator {

UserDefinedFunction::UserDefinedFunction(
    ast::Span span,
    Parameters parameters,
    std::shared_ptr<Body> body,
    std::weak_ptr<Scope> scope
)
    : Function(span), parameters(parameters), body(body), scope(scope) {}

ElementGuard UserDefinedFunction::call(CallFrame frame) const {
    if (this->parameters.parameters.size() != frame.arguments.size()) {
        std::string message;

        if (this->name() != "") {
            message = "Function `" + std::string(this->name()) + "` ";
        } else {
            message = "This lambda ";
        }

        message += "expects " +
                   std::to_string(this->parameters.parameters.size()) +
                   " arguments, got " + std::to_string(frame.arguments.size());

        throw EvaluationError(message, frame.call_site);
    }

    auto parent_scope = this->scope.lock();
    if (!parent_scope) {
        throw std::logic_error(
            "Failed to call a user-defined function because the garbage "
            "collector dropped its parent scope too early. This is a bug."
        );
    }
    auto scope = frame.context.garbage_collector->create_scope(parent_scope);

    for (size_t parameter_index = 0;
         parameter_index < this->parameters.parameters.size();
         parameter_index++) {
        scope->define(
            *this->parameters.parameters[parameter_index],
            frame.arguments[parameter_index]
        );
    }

    try {
        return this->body->evaluate(
            EvaluationContext(frame.context.garbage_collector, *scope)
        );
    } catch (ReturnControlFlow& e) {
        return std::move(e.element);
    }
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
