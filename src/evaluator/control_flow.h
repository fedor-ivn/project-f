#include <iostream>
#include <memory>

#include "expression.h"

namespace evaluator {

using ast::Element;

class BreakControlFlow {
  public:
    ElementGuard element;

    BreakControlFlow(ElementGuard element);
};

class ReturnControlFlow {
  public:
    ElementGuard element;

    ReturnControlFlow(ElementGuard element);
};

} // namespace evaluator
