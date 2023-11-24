#pragma once

#include "../ast/element.h"
#include "expression.h"

namespace evaluator {

class Evaluator {
    GarbageCollector garbage_collector;
    ScopeGuard global;

  public:
    Evaluator();

    ElementGuard evaluate(Program program);
};

} // namespace evaluator
