#include "evaluator.h"

namespace evaluator {

using ast::Element;

Evaluator::Evaluator(): global(std::make_shared<Scope>(Scope(nullptr))) {}

std::shared_ptr<Element> Evaluator::evaluate(Program program) {
    return program.evaluate(this->global);
}

} // namespace evaluator
