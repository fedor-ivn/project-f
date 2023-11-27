#include <memory>
#include <string>

#include "error.h"
#include "function.h"
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
    // For `std::make_shared`, `Scope`'s constructor is private 🤡, so
    // construct `shared_ptr` directly.
    std::shared_ptr<Scope> scope(new Scope(parent));
    this->alive_scopes.insert(scope);
    return ScopeGuard(this, scope);
}

ElementGuard GarbageCollector::temporary(std::shared_ptr<ast::Element> value) {
    if (auto function = std::dynamic_pointer_cast<UserDefinedFunction>(value)) {
        this->temporary_functions.insert(function);
    }
    return ElementGuard(this, value);
}

class ScopeVisitor {
  public:
    std::unordered_set<std::shared_ptr<Scope>> visited_scopes;
    std::unordered_set<std::shared_ptr<Scope>>* dead_scopes;
    std::unordered_set<std::shared_ptr<Scope>> next_dead_scopes;

    ScopeVisitor(std::unordered_set<std::shared_ptr<Scope>>* dead_scopes)
        : dead_scopes(dead_scopes) {}

    bool can_short_circuit() {
        return this->dead_scopes->size() == this->next_dead_scopes.size();
    }

    bool visit_scope(std::shared_ptr<Scope> scope) {
        if (this->visited_scopes.contains(scope)) {
            return false;
        }

        this->visited_scopes.insert(scope);

        if (this->dead_scopes->contains(scope)) {
            this->next_dead_scopes.insert(scope);
        }

        for (auto v : scope->variables) {
            if (auto function =
                    std::dynamic_pointer_cast<UserDefinedFunction>(v.second)) {
                if (this->visit_function(function)) {
                    return true;
                }
            }
        }

        if (scope->parent != nullptr) {
            return this->visit_scope(scope->parent);
        }

        return this->can_short_circuit();
    }

    bool visit_function(std::shared_ptr<UserDefinedFunction> function) {
        auto parent_scope = function->scope.lock();
        if (!parent_scope) {
            throw std::logic_error("Detected an early-collected parent scope "
                                   "of a function. This is a bug.");
        }

        return this->visit_scope(parent_scope);
    }
};

void GarbageCollector::collect() {
    if (this->dead_scopes.empty()) {
        return;
    }

    ScopeVisitor visitor(&this->dead_scopes);
    for (auto& function : this->temporary_functions) {
        if (visitor.visit_function(function)) {
            return;
        }
    }
    for (auto& scope : this->alive_scopes) {
        if (visitor.visit_scope(scope)) {
            return;
        }
    }

    this->dead_scopes = std::move(visitor.next_dead_scopes);
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

    if (auto function =
            std::dynamic_pointer_cast<UserDefinedFunction>(this->element)) {
        this->garbage_collector->temporary_functions.erase(function);
    }
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
