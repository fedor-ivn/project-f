#include "evaluator.h"
#include "function.h"

namespace evaluator {

using ast::Element;
using ast::Position;
using ast::Span;

Evaluator::Evaluator() : global(std::make_shared<Scope>(Scope(nullptr))) {
    Span nowhere(Position(0, 0), Position(0, 0));

    this->global->define(
        ast::Symbol("cons", nowhere), std::make_shared<ConsFunction>()
    );
}

std::shared_ptr<Element> Evaluator::evaluate(Program program) {
    return program.evaluate(this->global);
}

} // namespace evaluator
