#include "program.h"

namespace evaluator {

using ast::Element;
using ast::List;
using ast::Null;
using ast::Position;
using ast::Span;

std::unique_ptr<Expression>
Expression::from_element(std::unique_ptr<Element> element) {
    if (auto cons = element->to_cons()) {
        auto symbol = cons->left->to_symbol();

        if (symbol == "quote") {
            return Quote::parse(cons->right);
        }
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

void Atom::display(std::ostream& stream) const {
    stream << "Atom { element = " << this->atom->display_verbose() << "}";
}

Quote::Quote(std::shared_ptr<Element> element)
    : Expression(), element(element) {}

std::unique_ptr<Quote> Quote::parse(std::shared_ptr<List> arguments) {
    if (auto cons = arguments->to_cons()) {
        auto element = cons->left;

        if (cons->right->to_cons()) {
            throw std::runtime_error("quote has more than one argument");
        }

        return std::make_unique<Quote>(Quote(element));
    }

    throw std::runtime_error("quote has zero arguments");
}

std::shared_ptr<Element> Quote::evaluate() const { return this->element; }

void Quote::display(std::ostream& stream) const {
    stream << "Quote {\n";
    stream << "  element = " << this->element->display_verbose() << "\n";
    stream << "}";
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

std::ostream& operator<<(std::ostream& stream, const Program& program) {
    stream << "Program [\n";

    for (const auto& expression : program.program) {
        expression->display(stream);
        stream << ",\n";
    }

    stream << "]";
    return stream;
}

} // namespace evaluator
