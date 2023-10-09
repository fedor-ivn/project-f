#include "program.h"

namespace evaluator {

using ast::Element;
using ast::Null;
using ast::Position;
using ast::Span;

std::unique_ptr<Expression>
Expression::from_element(std::unique_ptr<Element> element) {
    if (element->to_cons()) {
        throw std::runtime_error("List evaluation is to be implemented");
    }

    return std::make_unique<Atom>(Atom(std::move(element)));
}

Atom::Atom(std::shared_ptr<Element> atom) : Expression(), atom(atom) {}

std::shared_ptr<Element> Atom::evaluate() const {
    if (this->atom->to_symbol()) {
        throw std::runtime_error("Symbol evaluation is to be implemented");
    }

    return this->atom;
}

Program::Program(std::vector<std::unique_ptr<Expression>> program)
    : program(std::move(program)) {}

Program Program::from_elements(std::vector<std::unique_ptr<Element>> elements) {
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

} // namespace evaluator
