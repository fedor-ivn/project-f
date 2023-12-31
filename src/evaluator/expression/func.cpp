#include "../../utils.h"
#include "../error.h"
#include "../expression.h"
#include "../function.h"

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
    std::shared_ptr<Body> body
)
    : Expression(span), name(name), parameters(std::move(parameters)),
      body(std::move(body)) {}

std::unique_ptr<Func> Func::parse(Span span, std::shared_ptr<List> arguments) {
    auto cons = to_cons(arguments);
    if (!cons) {
        throw EvaluationError(
            "`func` misses a function name, a parameter list and a body", span
        );
    }

    auto name = std::dynamic_pointer_cast<ast::Symbol>(cons->left);
    if (!name) {
        throw EvaluationError(
            "`func` expects a function name as its first argument",
            cons->left->span
        );
    }

    cons = to_cons(cons->right);
    if (!cons) {
        throw EvaluationError(
            "`func` misses a parameter list and a body", span
        );
    }

    auto parameter_list = std::dynamic_pointer_cast<List>(cons->left);
    if (!parameter_list) {
        throw EvaluationError(
            "`func` expects a parameter list as its second argument",
            cons->left->span
        );
    }
    auto parameters = Parameters::parse(parameter_list);

    auto body = Body::parse(cons->right);
    body.validate_no_free_break();

    return std::make_unique<Func>(
        span,
        name,
        std::move(parameters),
        std::make_shared<Body>(std::move(body))
    );
}

ElementGuard Func::evaluate(EvaluationContext context) const {
    auto function = std::make_shared<FuncFunction>(
        this->span,
        this->name->value,
        this->parameters,
        this->body,
        context.scope
    );

    context.scope->define(*this->name, function);

    return context.garbage_collector->temporary(function);
}

void Func::display(std::ostream& stream, size_t depth) const {
    stream << "Func {\n";

    stream << Depth(depth + 1)
           << "name = " << this->name->display_verbose(depth + 1) << '\n';

    stream << Depth(depth + 1) << "parameters = ";
    this->parameters.display(stream, depth + 1);
    stream << '\n';

    stream << Depth(depth + 1) << "body = ";
    this->body->display(stream, depth + 1);
    stream << '\n';

    stream << Depth(depth + 1) << "span = " << this->span << '\n';

    stream << Depth(depth) << '}';
}

bool Func::returns() const { return false; }
bool Func::breaks() const { return false; }
bool Func::can_evaluate_to(ast::ElementKind kind) const {
    return kind == ast::ElementKind::FUNCTION;
}
bool Func::can_break_with(ast::ElementKind) const { return false; }
void Func::validate_no_free_break() const {}
void Func::validate_no_break_with_value() const {}

} // namespace evaluator
