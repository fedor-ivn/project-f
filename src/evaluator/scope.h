#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "../ast/element.h"

namespace evaluator {

class Scope {
    std::unordered_map<std::string, std::shared_ptr<ast::Element>> variables;
    std::shared_ptr<Scope> parent;

  public:
    Scope(std::shared_ptr<Scope> parent);
    void define(ast::Symbol const& symbol, std::shared_ptr<ast::Element> value);
    std::shared_ptr<ast::Element> lookup(ast::Symbol const& symbol);
};

} // namespace scope
