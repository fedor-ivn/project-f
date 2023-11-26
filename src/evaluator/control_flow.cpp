#include "control_flow.h"

namespace evaluator {

BreakControlFlow::BreakControlFlow(std::shared_ptr<Element> element)
    : element(element){};

ReturnControlFlow::ReturnControlFlow(std::shared_ptr<Element> element)
    : element(element){};

} // namespace evaluator
