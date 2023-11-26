#include "control_flow.h"

BreakControlFlow::BreakControlFlow(std::shared_ptr<Element> element)
    : element(element){};

ReturnControlFlow::ReturnControlFlow(std::shared_ptr<Element> element)
    : element(element){};
