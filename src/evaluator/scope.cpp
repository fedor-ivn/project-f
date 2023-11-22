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

void Scope::set_or_define(
    ast::Symbol const& symbol, std::shared_ptr<ast::Element> value
) {
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

GarbageCollector::GarbageCollector() {}

ScopeGuard GarbageCollector::create_scope(std::shared_ptr<Scope> parent) {
    // For `std::make_shared`, `Scope`'s constructor is private 🤡, so keep
    // using the copy constructor here.
    auto scope = std::make_shared<Scope>(Scope(parent));
    this->alive_scopes.insert(scope);
    return ScopeGuard(this, scope);
}

ElementGuard GarbageCollector::temporary(std::shared_ptr<ast::Element> value) {
    this->temporaries.insert(value);
    return ElementGuard(this, value);
}

void GarbageCollector::collect() {
    // todo: must keep scopes which are held by functions still reachable
    // via alive scopes and temporaries
    this->dead_scopes.clear();
}

ScopeGuard::ScopeGuard(GarbageCollector* gc, std::shared_ptr<Scope> scope)
    : garbage_collector(gc), scope(scope) {}

ScopeGuard::~ScopeGuard() {
    if (!this->garbage_collector) {
        return;
    }

    this->garbage_collector->dead_scopes.insert(this->scope);
    this->garbage_collector->alive_scopes.erase(this->scope);
}

std::shared_ptr<Scope> ScopeGuard::operator*() { return this->scope; }
std::shared_ptr<Scope> ScopeGuard::operator->() { return this->scope; }

ElementGuard::ElementGuard(
    GarbageCollector* gc, std::shared_ptr<ast::Element> element
)
    : garbage_collector(gc), element(element) {}

ElementGuard::~ElementGuard() {
    if (!this->garbage_collector) {
        return;
    }

    this->garbage_collector->temporaries.erase(this->element);
    if (this->collect_garbage) {
        this->garbage_collector->collect();
    }
}

std::shared_ptr<ast::Element> ElementGuard::operator*() {
    return this->element;
}

std::shared_ptr<ast::Element> ElementGuard::operator->() {
    return this->element;
}

void ElementGuard::deactivate() { this->collect_garbage = false; }

} // namespace evaluator
