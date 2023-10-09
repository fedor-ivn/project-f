#include "evaluator.h"

namespace evaluator {

using ast::Element;

Evaluator::Evaluator() {}

std::shared_ptr<Element> Evaluator::evaluate(Program program) {
    return program.evaluate();
}

} // namespace evaluator
