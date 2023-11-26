#include "../../utils.h"
#include "../control_flow.h"
#include "../expression.h"
#include <memory>

namespace evaluator {

using ast::Element;
using utils::Depth;

Program::Program(Body body) : program(std::move(body)) {}

Program Program::parse(std::vector<std::shared_ptr<Element>> ast) {
    std::vector<std::unique_ptr<Expression>> program;
    for (auto& element : ast) {
        program.push_back(Expression::parse(element));
    }

    Body body(std::move(program));
    body.validate_no_free_break();
    return Program(std::move(body));
}

ElementGuard Program::evaluate(EvaluationContext context) const {
    try {
        return this->program.evaluate(context);
    } catch (ReturnControlFlow& e) {
        return std::move(e.element);
    }
}

std::ostream& operator<<(std::ostream& stream, Program const& self) {
    stream << "Program [\n";

    for (auto const& expression : self.program.body) {
        stream << Depth(1);
        expression->display(stream, 1);
        stream << ",\n";
    }

    stream << ']';

    return stream;
}

} // namespace evaluator
