#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "../ast/element.h"

namespace evaluator {

class UserDefinedFunction;

class Scope {
    std::unordered_map<std::string, std::shared_ptr<ast::Element>> variables;
    std::shared_ptr<Scope> parent;

    Scope(std::shared_ptr<Scope> parent);

    // Find the scope where the variable was defined.
    Scope* find_scope(ast::Symbol const& symbol);

  public:
    void define(ast::Symbol const& symbol, std::shared_ptr<ast::Element> value);
    void set_or_define(
        ast::Symbol const& symbol, std::shared_ptr<ast::Element> value
    );
    std::shared_ptr<ast::Element> lookup(ast::Symbol const& symbol);

    friend class GarbageCollector;
    friend class ScopeVisitor;
};

class ScopeGuard;
class ElementGuard;

class GarbageCollector {
    std::unordered_set<std::shared_ptr<Scope>> alive_scopes;
    std::unordered_set<std::shared_ptr<Scope>> dead_scopes;
    std::unordered_set<std::shared_ptr<UserDefinedFunction>>
        temporary_functions;

  public:
    GarbageCollector();

    ScopeGuard create_scope(std::shared_ptr<Scope> parent);
    ElementGuard temporary(std::shared_ptr<ast::Element> value);

    void collect();

    friend class ScopeGuard;
    friend class ElementGuard;
};

class ScopeGuard {
    GarbageCollector* garbage_collector;
    std::shared_ptr<Scope> scope;

    ScopeGuard(GarbageCollector*, std::shared_ptr<Scope>);

  public:
    ScopeGuard(ScopeGuard const&) = delete;
    ScopeGuard(ScopeGuard&&) = default;
    ~ScopeGuard();
    std::shared_ptr<Scope> operator*();
    std::shared_ptr<Scope> operator->();

    friend class GarbageCollector;
};

class ElementGuard {
    GarbageCollector* garbage_collector;
    std::shared_ptr<ast::Element> element;
    bool collect_garbage = true;

    ElementGuard(GarbageCollector*, std::shared_ptr<ast::Element>);

  public:
    ElementGuard(ElementGuard const&) = delete;
    ElementGuard(ElementGuard&&) = default;
    ~ElementGuard();
    std::shared_ptr<ast::Element> operator*();
    std::shared_ptr<ast::Element> operator->();

    // Still protect the element, but don't try to collect garbage when
    // destroyed
    void deactivate();

    friend class GarbageCollector;
};

} // namespace evaluator
