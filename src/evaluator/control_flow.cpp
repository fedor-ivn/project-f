#include "control_flow.h"

namespace evaluator {

BreakControlFlow::BreakControlFlow(ElementGuard element)
    : element(std::move(element)){};

ReturnControlFlow::ReturnControlFlow(ElementGuard element)
    : element(std::move(element)){};

} // namespace evaluator
