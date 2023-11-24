#include <iostream>
#include <memory>

#include "expression.h"

namespace evaluator {

class BreakControlFlow {
  public:
    ElementGuard element;

    BreakControlFlow(ElementGuard element);
};

} // namespace evaluator
