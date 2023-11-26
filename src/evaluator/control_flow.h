#include <iostream>
#include <memory>

#include "expression.h"

using ast::Element;

class BreakControlFlow {
  public:
    std::shared_ptr<Element> element;

    BreakControlFlow(std::shared_ptr<Element> element);
};

class ReturnControlFlow {
  public:
    std::shared_ptr<Element> element;

    ReturnControlFlow(std::shared_ptr<Element> element);
};
