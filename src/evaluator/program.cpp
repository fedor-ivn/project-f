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
        std::string type_name = typeid(T).name();
        std::string message;

        if (type_name == "N3ast6SymbolE") {
            message = "Expected type: Symbol";
        }
        else if (type_name == "N3ast4ListE") {
            message = "Expected type: List";
        }
        else {
            message = "Unexpected type" + type_name;
        }

        throw std::runtime_error(message);
    }

    return symbol;
}

std::vector<std::shared_ptr<ast::Symbol>> list_to_symbols_vector(std::shared_ptr<ast::List> list) {
    auto cons = list->to_cons();
    std::vector<std::shared_ptr<ast::Symbol>> symbols;

    while (cons) {
        auto element = cons->left;

        if (!element->to_symbol()) {
            throw std::runtime_error("Parameter is not a symbol");
        }

        auto symbol = std::make_shared<ast::Symbol>(
            ast::Symbol(std::string(element->to_symbol().value()), element->span)
        );

        symbols.push_back(symbol);

        cons = cons->right->to_cons();
    }

    return symbols;
}

void test_parameters(std::vector<std::shared_ptr<ast::Symbol>> parameters) {
    std::vector<std::string> set;

    for (auto parameter : parameters) {
        for (auto set_element : set) {
            if (parameter->value == set_element) {
                std::string message = "There is a duplicate argument: " + parameter->value;
                throw std::runtime_error(message);
            }

        }
        set.push_back(parameter->value);
    }
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
    std::vector<std::shared_ptr<ast::Symbol>> parameters,
    Program body) 
    : Expression(), name(name), parameters(parameters), body(std::move(body)) {}

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

    auto parameters_symbols = list_to_symbols_vector(func_arguments);
    test_parameters(parameters_symbols);

    if (!cons->right->to_cons()) {
        throw std::runtime_error("`func` takes at least 3 arguments, provided 2");
    }
    
    std::vector<std::shared_ptr<Element>> elements;

    while (cons) {
        auto element = std::shared_ptr<Element>(std::move(cons->left));
        elements.push_back(element);
        cons = cons->right->to_cons();
    }

    auto program = Program::from_elements(elements);

    return std::make_unique<Func>(Func(name, parameters_symbols, std::move(program)));
}

std::shared_ptr<ast::Element> Func::evaluate() const {
    throw std::runtime_error("Not implemented");
}

void Func::display(std::ostream& stream, size_t depth) const {
    stream << "Func {\n";

    stream << Depth(depth + 1)
           << "name = " << this->name->display_verbose(depth + 1) << '\n';

    stream << Depth(depth + 1) << "arguments = [\n";
    for (auto parameter : this->parameters) {
        stream << Depth(depth + 2) << parameter->display_verbose(depth+2);
        stream << ",\n";
    }
    stream << Depth(depth+1) << "]\n";

    stream << Depth(depth + 1) << "body = ";
    this->body.display(stream, depth + 1);
    stream << '\n';

    stream << Depth(depth) << '}';
}

Lambda::Lambda(
    std::vector<std::shared_ptr<ast::Symbol>> parameters, Program body
)
    : Expression(), parameters(parameters), body(std::move(body)) {}

std::unique_ptr<Lambda> Lambda::parse(std::shared_ptr<ast::List> arguments) {
    if (!arguments->to_cons()) {
        throw std::runtime_error("`lambda` takes at least 2 arguments, provided 0");
    }

    auto cons = arguments->to_cons();

    auto parameters_list = maybe_dynamic_cast<ast::List>(cons->left);
    auto parameters = list_to_symbols_vector(parameters_list);
    test_parameters(parameters);

    cons = cons->right->to_cons();

    if (!cons) {
        throw std::runtime_error("`lambda` takes at least 2 arguments, provided 1");
    }
    
    std::vector<std::shared_ptr<Element>> elements;

    while (cons) {
        auto element = std::shared_ptr<Element>(std::move(cons->left));
        elements.push_back(element);
        cons = cons->right->to_cons();
    }

    auto program = Program::from_elements(elements);

    return std::make_unique<Lambda>(Lambda(parameters, std::move(program)));
}

std::shared_ptr<ast::Element> Lambda::evaluate() const {
    throw std::runtime_error("Not implemented");
}

void Lambda::display(std::ostream& stream, size_t depth) const {
    stream << "Lambda {\n";

    stream << Depth(depth + 1) << "arguments = [\n";
    for (auto parameter : this->parameters) {
        stream << Depth(depth + 2) << parameter->display_verbose(depth+2);
        stream << ",\n";
    }
    stream << Depth(depth+1) << "]\n";

    stream << Depth(depth + 1) << "body = ";
    this->body.display(stream, depth + 1);
    stream << '\n';

    stream << Depth(depth) << '}';
}

Prog::Prog(
    std::vector<std::shared_ptr<ast::Symbol>> parameters, std::unique_ptr<Expression> expression
)
    : Expression(), parameters(parameters), expression(std::move(expression)){};

std::unique_ptr<Prog> Prog::parse(std::shared_ptr<ast::List> arguments) {
    if (!arguments->to_cons()) {
        throw std::runtime_error("`prog` takes at least 2 arguments, provided 0"
        );
    }

    auto cons = arguments->to_cons();

    auto parameters_list = maybe_dynamic_cast<ast::List>(cons->left);
    auto parameters = list_to_symbols_vector(parameters_list);
    test_parameters(parameters);

    if (!cons->right->to_cons()) {
        throw std::runtime_error("`prog` takes at least 2 arguments, provided 1"
        );
    }

    auto body_element = std::static_pointer_cast<ast::Element>(cons->right);
    auto body = Expression::from_element(body_element);

    return std::make_unique<Prog>(Prog(parameters, std::move(body)));
}

std::shared_ptr<ast::Element> Prog::evaluate() const {
    throw std::runtime_error("Not implemented");
}

void Prog::display(std::ostream& stream, size_t depth) const {
    stream << "Prog {\n";

    stream << Depth(depth + 1) << "arguments = [\n";
    for (auto parameter : this->parameters) {
        stream << Depth(depth + 2) << parameter->display_verbose(depth+2);
        stream << ",\n";
    }
    stream << Depth(depth+1) << "]\n";

    stream << Depth(depth + 1) << "body = ";
    this->expression->display(stream, depth + 1);
    stream << '\n';

    stream << Depth(depth) << '}';
}

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

} // namespace evaluator
