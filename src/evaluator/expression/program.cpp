#include "../../utils.h"
#include "../error.h"
#include "../expression.h"

namespace evaluator {

using ast::Element;
using ast::Null;
using ast::Position;
using ast::Span;
using utils::Depth;

Program::Program(std::vector<std::unique_ptr<Expression>> program)
    : program(std::move(program)) {}

Program Program::from_elements(std::vector<std::shared_ptr<Element>> elements) {
    std::vector<std::unique_ptr<Expression>> program;
    for (auto&& element : elements) {
        program.push_back(Expression::from_element(std::move(element)));
    }
    return Program(std::move(program));
}

std::shared_ptr<Element> Program::evaluate() const {
    std::shared_ptr<Element> last_evaluated =
        std::make_shared<Null>(Null(Span(Position(0, 0), Position(0, 0))));

    for (auto& expression : this->program) {
        last_evaluated = expression->evaluate();
    }
    return last_evaluated;
}

void Program::display(std::ostream& stream, size_t depth) const {
    stream << "Program [\n";

    for (auto const& expression : this->program) {
        stream << Depth(depth + 1);
        expression->display(stream, depth + 1);
        stream << ",\n";
    }

    stream << Depth(depth) << ']';
}

std::ostream& operator<<(std::ostream& stream, Program const& program) {
    program.display(stream, 0);
    return stream;
}

bool Program::can_evaluate_to_function() const {
    return this->program[this->program.size() - 1]->can_evaluate_to_function();
}

bool Program::can_evaluate_to_boolean() const {
    return this->program[this->program.size() - 1]->can_evaluate_to_boolean();
}

} // namespace evaluator
