#include "program.h"
#include <memory>
#include <stdexcept>

#include "../utils.h"
#include "error.h"

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
            message = "expected type - Symbol";
        } else if (type_name == "N3ast4ListE") {
            message = "expected type - List";
        } else if (type_name == "N3ast7BooleanE") {
            message = "expected type - Boolean";
        } else {
            message = "unexpected type - " + type_name;
        }

        throw EvaluationError(message, element->span);
    }

    return symbol;
}

std::vector<std::shared_ptr<ast::Symbol>>
list_to_symbols_vector(std::shared_ptr<ast::List> list) {
    auto cons = list->to_cons();
    std::vector<std::shared_ptr<ast::Symbol>> symbols;

    while (cons) {
        auto element = cons->left;

        if (!element->to_symbol()) {
            throw EvaluationError("parameter is not a symbol", element->span);
        }

        auto symbol = maybe_dynamic_cast<ast::Symbol>(element);

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
                std::string message =
                    "parameter `" + parameter->value + "` is duplicated";
                throw EvaluationError(message, parameter->span);
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

        else if (symbol == "return") {
            return Return::parse(arguments);
        }

        else if (symbol == "while") {
            return While::parse(arguments);
        }

        else if (symbol == "break") {
            return Break::parse(arguments);
        }

        else if (symbol == "cond") {
            return Cond::parse(arguments);
        }

        return Call::parse(*cons);
    }

    if (auto symbol = std::dynamic_pointer_cast<ast::Symbol>(element)) {
        return std::make_unique<Symbol>(Symbol(std::move(symbol)));
    }

    return std::make_unique<Quote>(Quote(std::move(element)));
}

Symbol::Symbol(std::shared_ptr<ast::Symbol> symbol)
    : Expression(), symbol(symbol) {}

std::shared_ptr<Element> Symbol::evaluate() const {
    throw std::runtime_error("Symbol evaluation is to be implemented");
}

void Symbol::display(std::ostream& stream, size_t depth) const {
    stream << "Symbol {\n";
    stream << Depth(depth + 1)
           << "symbol = " << this->symbol->display_verbose(depth + 1) << '\n';
    stream << Depth(depth) << '}';
}

bool Symbol::can_evaluate_to_function() const { return true; }

Quote::Quote(std::shared_ptr<Element> element)
    : Expression(), element(element) {}

std::unique_ptr<Quote> Quote::parse(std::shared_ptr<List> arguments) {
    auto form_span = arguments->span;
    auto cons = arguments->to_cons();
    if (!cons) {
        throw EvaluationError("`quote` is empty", form_span);
    }

    auto element = cons->left;

    cons = cons->right->to_cons();
    if (cons) {
        throw EvaluationError("`quote` has extra arguments", cons->span);
    }

    return std::make_unique<Quote>(Quote(element));
}

std::shared_ptr<Element> Quote::evaluate() const { return this->element; }

void Quote::display(std::ostream& stream, size_t depth) const {
    stream << "Quote {\n";
    stream << Depth(depth + 1)
           << "element = " << this->element->display_verbose(depth + 1) << '\n';
    stream << Depth(depth) << '}';
}

bool Quote::can_evaluate_to_function() const { return false; }

Setq::Setq(
    std::shared_ptr<ast::Symbol> symbol, std::unique_ptr<Expression> expression
)
    : Expression(), variable(symbol), initializer(std::move(expression)) {}

std::unique_ptr<Setq> Setq::parse(std::shared_ptr<ast::List> arguments) {
    auto form_span = arguments->span;
    auto cons = arguments->to_cons();
    if (!cons) {
        throw EvaluationError(
            "`setq` without a variable name and an initializer", form_span
        );
    }

    auto symbol = maybe_dynamic_cast<ast::Symbol>(cons->left);

    cons = cons->right->to_cons();
    if (!cons) {
        throw EvaluationError("`setq` without an initializer", form_span);
    }

    auto expression = Expression::from_element(cons->left);

    cons = cons->right->to_cons();
    if (cons) {
        throw EvaluationError("`setq` has extra arguments", cons->span);
    }

    return std::make_unique<Setq>(Setq(symbol, std::move(expression)));
}

std::shared_ptr<ast::Element> Setq::evaluate() const {
    throw std::runtime_error("Not implemented");
}

bool Setq::can_evaluate_to_function() const {
    return this->initializer->can_evaluate_to_function();
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
    Program body
)
    : Expression(), name(name), parameters(parameters), body(std::move(body)) {}

std::unique_ptr<Func> Func::parse(std::shared_ptr<ast::List> arguments) {
    if (!arguments->to_cons()) {
        throw EvaluationError(
            "`func` needs a function name, a parameter list and a body", arguments->span
        );
    }

    auto cons = arguments->to_cons();

    auto name = maybe_dynamic_cast<ast::Symbol>(cons->left);

    if (!cons->right->to_cons()) {
        throw EvaluationError(
            "`func` needs a parameter list and a body", cons->span
        );
    }
    cons = cons->right->to_cons();

    auto func_arguments = maybe_dynamic_cast<ast::List>(cons->left);

    auto parameters_symbols = list_to_symbols_vector(func_arguments);
    test_parameters(parameters_symbols);

    if (!cons->right->to_cons()) {
        throw EvaluationError(
            "`func` needs a body", cons->span
        );
    }

    std::vector<std::shared_ptr<Element>> elements;

    while (cons) {
        elements.push_back(cons->left);
        cons = cons->right->to_cons();
    }

    auto program = Program::from_elements(elements);

    return std::make_unique<Func>(
        Func(name, parameters_symbols, std::move(program))
    );
}

std::shared_ptr<ast::Element> Func::evaluate() const {
    throw std::runtime_error("Not implemented");
}

bool Func::can_evaluate_to_function() const { return true; }

void Func::display(std::ostream& stream, size_t depth) const {
    stream << "Func {\n";

    stream << Depth(depth + 1)
           << "name = " << this->name->display_verbose(depth + 1) << '\n';

    stream << Depth(depth + 1) << "arguments = [\n";
    for (auto parameter : this->parameters) {
        stream << Depth(depth + 2) << parameter->display_verbose(depth + 2);
        stream << ",\n";
    }
    stream << Depth(depth + 1) << "]\n";

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
        throw EvaluationError(
            "`lambda` needs a parameter list and a body", arguments->span
        );
    }

    auto cons = arguments->to_cons();

    auto parameters_list = maybe_dynamic_cast<ast::List>(cons->left);
    auto parameters = list_to_symbols_vector(parameters_list);
    test_parameters(parameters);

    if (!cons->right->to_cons()) {
        throw EvaluationError(
            "`lambda` takes at least 2 arguments, provided 1", cons->span
        );
    }
    cons = cons->right->to_cons();

    std::vector<std::shared_ptr<Element>> elements;

    while (cons) {
        auto element = std::shared_ptr<Element>(cons->left);
        elements.push_back(element);
        cons = cons->right->to_cons();
    }

    auto program = Program::from_elements(elements);

    return std::make_unique<Lambda>(Lambda(parameters, std::move(program)));
}

std::shared_ptr<ast::Element> Lambda::evaluate() const {
    throw std::runtime_error("Not implemented");
}

bool Lambda::can_evaluate_to_function() const { return true; }

void Lambda::display(std::ostream& stream, size_t depth) const {
    stream << "Lambda {\n";

    stream << Depth(depth + 1) << "arguments = [\n";
    for (auto parameter : this->parameters) {
        stream << Depth(depth + 2) << parameter->display_verbose(depth + 2);
        stream << ",\n";
    }
    stream << Depth(depth + 1) << "]\n";

    stream << Depth(depth + 1) << "body = ";
    this->body.display(stream, depth + 1);
    stream << '\n';

    stream << Depth(depth) << '}';
}

Prog::Prog(std::vector<std::shared_ptr<ast::Symbol>> variables, Program body)
    : Expression(), variables(variables), body(std::move(body)){};

std::unique_ptr<Prog> Prog::parse(std::shared_ptr<ast::List> arguments) {
    if (!arguments->to_cons()) {
        throw EvaluationError(
            "`prog` takes at least 2 arguments, provided 0", arguments->span
        );
    }

    auto cons = arguments->to_cons();

    auto parameters_list = maybe_dynamic_cast<ast::List>(cons->left);
    auto parameters = list_to_symbols_vector(parameters_list);
    test_parameters(parameters);

    if (!cons->right->to_cons()) {
        throw EvaluationError(
            "`prog` needs a body", cons->span
        );
    }
    cons = cons->right->to_cons();

    std::vector<std::shared_ptr<Element>> elements;

    while (cons) {
        auto element = std::shared_ptr<Element>(cons->left);
        elements.push_back(element);
        cons = cons->right->to_cons();
    }

    auto program = Program::from_elements(elements);

    return std::make_unique<Prog>(Prog(parameters, std::move(program)));
}

std::shared_ptr<ast::Element> Prog::evaluate() const {
    throw std::runtime_error("Not implemented");
}

void Prog::display(std::ostream& stream, size_t depth) const {
    stream << "Prog {\n";

    stream << Depth(depth + 1) << "arguments = [\n";
    for (auto parameter : this->variables) {
        stream << Depth(depth + 2) << parameter->display_verbose(depth + 2);
        stream << ",\n";
    }
    stream << Depth(depth + 1) << "]\n";

    stream << Depth(depth + 1) << "body = ";
    this->body.display(stream, depth + 1);
    stream << '\n';

    stream << Depth(depth) << '}';
}

bool Program::can_evaluate_to_function() const {
    return this->program[this->program.size() - 1]->can_evaluate_to_function();
}

bool Prog::can_evaluate_to_function() const {
    return this->body.can_evaluate_to_function();
}

Return::Return(std::unique_ptr<Expression> expression)
    : Expression(), expression(std::move(expression)) {}

std::unique_ptr<Return> Return::parse(std::shared_ptr<ast::List> arguments) {
    auto cons = arguments->to_cons();
    if (!cons) {
        auto null = Quote(std::make_shared<Null>(Null(arguments->span)));
        return std::make_unique<Return>(Return(std::make_unique<Quote>(null)));
    }

    auto expression = Expression::from_element(cons->left);

    cons = cons->right->to_cons();
    if (cons) {
        throw EvaluationError("`return` has extra arguments", cons->span);
    }

    return std::make_unique<Return>(Return(std::move(expression)));
}

std::shared_ptr<ast::Element> Return::evaluate() const {
    throw std::runtime_error("Not implemented");
}

bool Return::can_evaluate_to_function() const { return false; }

void Return::display(std::ostream& stream, size_t depth) const {
    stream << "Return {\n";

    stream << Depth(depth + 1) << "expression = ";
    this->expression->display(stream, depth + 1);
    stream << '\n';

    stream << Depth(depth) << '}';
}

While::While(std::unique_ptr<Expression> condition, Program body)
    : Expression(), condition(std::move(condition)), body(std::move(body)) {}

std::unique_ptr<While> While::parse(std::shared_ptr<ast::List> arguments) {
    auto form_span = arguments->span;
    auto cons = arguments->to_cons();
    if (!cons) {
        throw EvaluationError("`while` is empty", form_span);
    }

    auto condition = Expression::from_element(cons->left);

    cons = cons->right->to_cons();
    if (!cons) {
        throw EvaluationError("`while` without a body", form_span);
    }

    std::vector<std::shared_ptr<Element>> elements;
    while (cons) {
        elements.push_back(cons->left);
        cons = cons->right->to_cons();
    }
    auto program = Program::from_elements(elements);

    return std::make_unique<While>(
        While(std::move(condition), std::move(program))
    );
}

std::shared_ptr<ast::Element> While::evaluate() const {
    throw std::runtime_error("Not implemented");
}

void While::display(std::ostream& stream, size_t depth) const {
    stream << "While {\n";

    stream << Depth(depth + 1) << "condition = ";
    this->condition->display(stream, depth + 1);
    stream << '\n';

    stream << Depth(depth + 1) << "body = ";
    this->body.display(stream, depth + 1);
    stream << '\n';

    stream << Depth(depth) << '}';
}

bool While::can_evaluate_to_function() const { return false; }

Break::Break(std::unique_ptr<Expression> expression)
    : Expression(), expression(std::move(expression)) {}

std::unique_ptr<Break> Break::parse(std::shared_ptr<ast::List> arguments) {
    auto cons = arguments->to_cons();
    if (!cons) {
        auto null = Quote(std::make_shared<Null>(Null(arguments->span)));
        return std::make_unique<Break>(Break(std::make_unique<Quote>(null)));
    }

    auto expression = Expression::from_element(cons->left);

    cons = cons->right->to_cons();
    if (cons) {
        throw EvaluationError("`break` has extra arguments", cons->span);
    }

    return std::make_unique<Break>(Break(std::move(expression)));
}

std::shared_ptr<ast::Element> Break::evaluate() const {
    throw std::runtime_error("Not implemented");
}

void Break::display(std::ostream& stream, size_t depth) const {
    stream << "Break {\n";

    stream << Depth(depth + 1) << "expression = ";
    this->expression->display(stream, depth + 1);
    stream << '\n';

    stream << Depth(depth) << '}';
}

bool Break::can_evaluate_to_function() const { return false; }

Call::Call(
    std::unique_ptr<Expression> function,
    std::vector<std::unique_ptr<Expression>> arguments
)
    : function(std::move(function)), arguments(std::move(arguments)) {}

std::unique_ptr<Call> Call::parse(ast::Cons const& form) {
    auto function = Expression::from_element(form.left);
    if (!function->can_evaluate_to_function()) {
        throw EvaluationError(
            "function (or a special form) expected, but this expression will "
            "never evaluate to a function",
            form.left->span
        );
    }

    std::vector<std::unique_ptr<Expression>> arguments;
    auto cons = form.right->to_cons();
    while (cons) {
        arguments.push_back(Expression::from_element(cons->left));
        cons = cons->right->to_cons();
    }

    return std::make_unique<Call>(
        Call(std::move(function), std::move(arguments))
    );
}

std::shared_ptr<Element> Call::evaluate() const {
    throw std::runtime_error("Not implemented");
}

void Call::display(std::ostream& stream, size_t depth) const {
    stream << "Call {\n";

    stream << Depth(depth + 1) << "function = ";
    this->function->display(stream, depth + 1);
    stream << '\n';

    stream << Depth(depth + 1) << "arguments = [\n";
    for (auto const& argument : this->arguments) {
        stream << Depth(depth + 2);
        argument->display(stream, depth + 2);
        stream << ",\n";
    }
    stream << Depth(depth + 1) << "]\n";

    stream << Depth(depth) << '}';
}

bool Call::can_evaluate_to_function() const { return true; }

Cond::Cond(
    std::unique_ptr<Expression> condition,
    std::unique_ptr<Expression> then,
    std::unique_ptr<Expression> otherwise
)
    : Expression(), condition(std::move(condition)), then(std::move(then)),
      otherwise(std::move(otherwise)) {}

std::unique_ptr<Cond> Cond::parse(std::shared_ptr<ast::List> arguments) {
    auto cons = arguments->to_cons();

    if (!cons) {
        throw EvaluationError(
            "`cond` takes 2 or 3 arguments, provided 0", arguments->span
        );
    }

    auto condition = Expression::from_element(cons->left);

    if (!cons->right->to_cons()) {
        throw EvaluationError(
            "`cond` needs at least a `then` branch", cons->span
        );
    }
    cons = cons->right->to_cons();

    auto then = Expression::from_element(cons->left);

    cons = cons->right->to_cons();

    std::unique_ptr<Expression> otherwise;
    if (cons) {
        otherwise = Expression::from_element(cons->left);

        if (cons->right->to_cons()) {
            throw EvaluationError(
                "`cond` takes 2 or 3 arguments, provided more than 3",
                cons->span
            );
        }
    } else {
        otherwise = Expression::from_element(
            std::make_shared<Null>(Null(arguments->span))
        );
    }

    auto cond =
        Cond(std::move(condition), std::move(then), std::move(otherwise));
    return std::make_unique<Cond>(std::move(cond));
}

std::shared_ptr<Element> Cond::evaluate() const {
    throw std::runtime_error("Not implemented");
}

void Cond::display(std::ostream& stream, size_t depth) const {
    stream << "Cond {\n";

    stream << Depth(depth + 1) << "condition = ";
    this->condition->display(stream, depth + 1);
    stream << '\n';

    stream << Depth(depth + 1) << "then = ";
    this->then->display(stream, depth + 1);
    stream << '\n';

    stream << Depth(depth + 1) << "otherwise = ";
    this->otherwise->display(stream, depth + 1);
    stream << "\n";

    stream << Depth(depth) << '}';
}

bool Cond::can_evaluate_to_function() const {
    return this->then->can_evaluate_to_function() ||
           this->otherwise->can_evaluate_to_function();
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
