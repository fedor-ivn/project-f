#include "../../utils.h"
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

std::shared_ptr<Element> Program::evaluate(std::shared_ptr<Scope> parent) const {
    return this->program.evaluate(parent);
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
