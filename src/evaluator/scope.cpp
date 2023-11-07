#include <memory>
#include <string>

#include "error.h"
#include "scope.h"

namespace evaluator {

Scope::Scope(std::shared_ptr<Scope> parent) : parent(parent) {}

void Scope::define(
    ast::Symbol const& symbol, std::shared_ptr<ast::Element> value
) {
    this->variables[symbol.value] = value;
}

std::shared_ptr<ast::Element> Scope::lookup(ast::Symbol const& symbol) {
    if (this->variables.contains(symbol.value)) {
        return this->variables[symbol.value];
    }
    if (this->parent != nullptr) {
        return this->parent->lookup(symbol);
    }
    throw EvaluationError(
        "Undefined variable: " + symbol.value, symbol.span
    );
}

} // namespace evaluator
