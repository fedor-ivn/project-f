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

Scope* Scope::find_scope(ast::Symbol const& symbol) {
    if (this->variables.contains(symbol.value)) {
        return this;
    }
    if (this->parent != nullptr) {
        return this->parent->find_scope(symbol);
    }
    return nullptr;
}

void Scope::set_or_define(ast::Symbol const& symbol, std::shared_ptr<ast::Element> value) {
    if (auto found = this->find_scope(symbol)) {
        found->define(symbol, value);
        return;
    }
    this->define(symbol, value);
}

std::shared_ptr<ast::Element> Scope::lookup(ast::Symbol const& symbol) {
    if (this->variables.contains(symbol.value)) {
        return this->variables[symbol.value];
    }
    if (this->parent != nullptr) {
        return this->parent->lookup(symbol);
    }
    throw EvaluationError(
        "variable `" + symbol.value + "` is not defined", symbol.span
    );
}

} // namespace evaluator