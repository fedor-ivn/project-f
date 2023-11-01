#include "program.h"
#include <memory>
#include <stdexcept>

#include "../utils.h"

namespace evaluator {

using ast::Element;
using ast::List;
using ast::Null;
using ast::Position;
using ast::Span;
using utils::Depth;

template <typename T>
std::shared_ptr<T> maybe_dynamic_cast(std::shared_ptr<ast::Element> element) {
    auto symbol = std::dynamic_pointer_cast<T>(element);

    if (symbol == nullptr) {
        std::string message = typeid(T).name();
        throw std::runtime_error("Expected type: " + message);
    }

    return symbol;
}

std::unique_ptr<Expression>
Expression::from_element(std::shared_ptr<Element> element) {
    if (auto cons = element->to_cons()) {
        auto symbol = cons->left->to_symbol();
        auto arguments = cons->right;

        if (symbol == "quote") {
            return Quote::parse(arguments);
        }

        else if (symbol == "setq") {
            return Setq::parse(arguments);
        }

        else if (symbol == "func") {
            return Func::parse(arguments);
        }

        else if (symbol == "lambda") {
            return Lambda::parse(arguments);
        }

        else if (symbol == "prog") {
            return Prog::parse(arguments);
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

void Atom::display(std::ostream& stream, size_t depth) const {
    stream << "Atom {\n";
    stream << Depth(depth + 1)
           << "element = " << this->atom->display_verbose(depth + 1) << '\n';
    stream << Depth(depth) << '}';
}

Quote::Quote(std::shared_ptr<Element> element)
    : Expression(), element(element) {}

std::unique_ptr<Quote> Quote::parse(std::shared_ptr<List> arguments) {
    if (auto cons = arguments->to_cons()) {
        auto element = cons->left;

        if (cons->right->to_cons()) {
            throw std::runtime_error(
                "`quote` takes 1 argument, provided more than one"
            );
        }

        return std::make_unique<Quote>(Quote(element));
    }

    throw std::runtime_error("`quote` takes 1 argument, provided 0");
}

std::shared_ptr<Element> Quote::evaluate() const { return this->element; }

void Quote::display(std::ostream& stream, size_t depth) const {
    stream << "Quote {\n";
    stream << Depth(depth + 1)
           << "element = " << this->element->display_verbose(depth + 1) << '\n';
    stream << Depth(depth) << '}';
}

Setq::Setq(
    std::shared_ptr<ast::Symbol> symbol, std::unique_ptr<Expression> expression
)
    : Expression(), variable(symbol), initializer(std::move(expression)) {}

std::unique_ptr<Setq> Setq::parse(std::shared_ptr<ast::List> arguments) {
    if (!arguments->to_cons()) {
        throw std::runtime_error("`setq` takes 2 arguments, provided 0");
    }

    auto cons = arguments->to_cons();

    auto symbol = maybe_dynamic_cast<ast::Symbol>(cons->left);

    if (!cons->right->to_cons()) {
        throw std::runtime_error("`setq` takes 2 arguments, provided 1");
    }

    auto expression = Expression::from_element(cons->right->to_cons()->left);

    if (cons->right->to_cons()->right->to_cons()) {
        throw std::runtime_error(
            "`setq` takes 2 arguments, provided more than two"
        );
    }

    return std::make_unique<Setq>(Setq(symbol, std::move(expression)));
}

std::shared_ptr<ast::Element> Setq::evaluate() const {
    throw std::runtime_error("Not implemented");
}

void Setq::display(std::ostream& stream, size_t depth) const {
    stream << "Setq {\n";

    stream << Depth(depth + 1)
           << "variable = " << this->variable->display_verbose(depth + 1)
           << '\n';

    stream << Depth(depth + 1) << "initializer = ";
    this->initializer->display(stream, depth + 1);
    stream << '\n';

    stream << Depth(depth) << '}';
}

Func::Func(
    std::shared_ptr<ast::Symbol> name,
    std::shared_ptr<ast::List> arguments,
    std::shared_ptr<Program> program) 
    : Expression(), name(name), arguments(arguments), program(program) {}

std::unique_ptr<Func> Func::parse(std::shared_ptr<ast::List> arguments) {
    if (!arguments->to_cons()) {
        throw std::runtime_error("`func` takes at least 3 arguments, provided 0"
        );
    }

    auto cons = arguments->to_cons();

    auto name = maybe_dynamic_cast<ast::Symbol>(cons->left);

    cons = cons->right->to_cons();

    if (!cons) {
        throw std::runtime_error("`func` takes at least 3 arguments, provided 1");
    }

    auto func_arguments = maybe_dynamic_cast<ast::List>(cons->left);

    if (!cons->right->to_cons()) {
        throw std::runtime_error("`func` takes at least 3 arguments, provided 2");
    }
    
    std::vector<std::shared_ptr<Element>> elements;

    while (cons) {
        auto element = std::shared_ptr<Element>(std::move(cons->left));
        elements.push_back(element);
        cons = cons->right->to_cons();
    }

    auto program = std::make_shared<Program>(Program::from_elements(elements));

    return std::make_unique<Func>(Func(name, func_arguments, program));
}

std::shared_ptr<ast::Element> Func::evaluate() const {
    throw std::runtime_error("Not implemented");
}

void Func::display(std::ostream& stream, size_t depth) const {
    stream << "Func {\n";

    stream << Depth(depth + 1)
           << "name = " << this->name->display_verbose(depth + 1) << '\n';

    stream << Depth(depth + 1)
           << "arguments = " << this->arguments->display_verbose(depth + 1)
           << '\n';

    stream << Depth(depth + 1) << "body = ";
    this->program->display(stream, depth + 1);
    stream << '\n';

    stream << Depth(depth) << '}';
}

Lambda::Lambda(
    std::shared_ptr<ast::List> arguments, std::unique_ptr<Expression> expression
)
    : Expression(), arguments(arguments), expression(std::move(expression)) {}

std::unique_ptr<Lambda> Lambda::parse(std::shared_ptr<ast::List> arguments) {
    if (!arguments->to_cons()) {
        throw std::runtime_error(
            "`lambda` takes at least 2 arguments, provided 0"
        );
    }

    auto cons = arguments->to_cons();

    auto func_arguments = maybe_dynamic_cast<ast::List>(cons->left);

    if (!cons->right->to_cons()) {
        throw std::runtime_error(
            "`lambda` takes at least 2 arguments, provided 1"
        );
    }

    auto body_element = std::static_pointer_cast<ast::Element>(cons->right);
    auto body = Expression::from_element(body_element);

    return std::make_unique<Lambda>(Lambda(func_arguments, std::move(body)));
}

std::shared_ptr<ast::Element> Lambda::evaluate() const {
    throw std::runtime_error("Not implemented");
}

void Lambda::display(std::ostream& stream, size_t depth) const {
    stream << "Lambda {\n";

    stream << Depth(depth + 1)
           << "arguments = " << this->arguments->display_verbose(depth + 1)
           << '\n';

    stream << Depth(depth + 1) << "body = ";
    this->expression->display(stream, depth + 1);
    stream << '\n';

    stream << Depth(depth) << '}';
}

Prog::Prog(
    std::shared_ptr<ast::List> arguments, std::unique_ptr<Expression> expression
)
    : Expression(), variables(arguments), expression(std::move(expression)){};

std::unique_ptr<Prog> Prog::parse(std::shared_ptr<ast::List> arguments) {
    if (!arguments->to_cons()) {
        throw std::runtime_error("`prog` takes at least 2 arguments, provided 0"
        );
    }

    auto cons = arguments->to_cons();

    auto func_arguments = maybe_dynamic_cast<ast::List>(cons->left);

    if (!cons->right->to_cons()) {
        throw std::runtime_error("`prog` takes at least 2 arguments, provided 1"
        );
    }

    auto body_element = std::static_pointer_cast<ast::Element>(cons->right);
    auto body = Expression::from_element(body_element);

    return std::make_unique<Prog>(Prog(func_arguments, std::move(body)));
}

std::shared_ptr<ast::Element> Prog::evaluate() const {
    throw std::runtime_error("Not implemented");
}

void Prog::display(std::ostream& stream, size_t depth) const {
    stream << "Prog {\n";

    stream << Depth(depth + 1)
           << "variables = " << this->variables->display_verbose(depth + 1)
           << '\n';

    stream << Depth(depth + 1) << "body = ";
    this->expression->display(stream, depth + 1);
    stream << '\n';

    stream << Depth(depth) << '}';
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

} // namespace evaluator
