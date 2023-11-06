#pragma once

#include "../ast/element.h"
#include "expression.h"

namespace evaluator {

class Evaluator {
  public:
    Evaluator();

    std::shared_ptr<ast::Element> evaluate(Program program);
};

} // namespace evaluator
