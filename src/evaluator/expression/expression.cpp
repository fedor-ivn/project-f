#include "../expression.h"

namespace evaluator {

using ast::Cons;
using ast::Element;
using ast::Span;

EvaluationContext::EvaluationContext(
    GarbageCollector* gc, std::shared_ptr<Scope> scope
)
    : garbage_collector(gc), scope(scope) {}

Expression::Expression(Span span) : span(span) {}

std::unique_ptr<Expression> Expression::parse(std::shared_ptr<Element> element
) {
    if (auto cons = std::dynamic_pointer_cast<Cons>(element)) {
        auto symbol = std::dynamic_pointer_cast<ast::Symbol>(cons->left);
        if (symbol) {
            std::string_view name(symbol->value);
            auto arguments = cons->right;

            if (name == "quote") {
                return Quote::parse(cons->span, arguments);
            }

            if (name == "setq") {
                return Setq::parse(cons->span, arguments);
            }

            if (name == "func") {
                return Func::parse(cons->span, arguments);
            }

            if (name == "lambda") {
                return Lambda::parse(cons->span, arguments);
            }

            if (name == "prog") {
                return Prog::parse(cons->span, arguments);
            }

            if (name == "return") {
                return Return::parse(cons->span, arguments);
            }

            if (name == "while") {
                return While::parse(cons->span, arguments);
            }

            if (name == "break") {
                return Break::parse(cons->span, arguments);
            }

            if (name == "cond") {
                return Cond::parse(cons->span, arguments);
            }
        }

        return Call::parse(cons);
    }

    if (auto symbol = std::dynamic_pointer_cast<ast::Symbol>(element)) {
        return std::make_unique<Symbol>(Symbol(std::move(symbol)));
    }

    return std::make_unique<Quote>(Quote(element->span, element));
}

bool Expression::diverges() const { return this->returns() || this->breaks(); }

} // namespace evaluator
