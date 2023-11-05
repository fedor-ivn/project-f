#pragma once

#include "../ast/element.h"
#include <memory>
#include <vector>

namespace evaluator {

class Expression {
  public:
    static std::unique_ptr<Expression>
    from_element(std::shared_ptr<ast::Element> element);

    virtual ~Expression() = default;

    virtual std::shared_ptr<ast::Element> evaluate() const = 0;
    virtual void display(std::ostream& stream, size_t depth) const = 0;

    virtual bool can_evaluate_to_function() const = 0;
    virtual bool can_evaluate_to_boolean() const = 0;
};

class Symbol : public Expression {
    std::shared_ptr<ast::Symbol> symbol;

  public:
    Symbol(std::shared_ptr<ast::Symbol> symbol);

    virtual std::shared_ptr<ast::Element> evaluate() const;
    virtual void display(std::ostream& stream, size_t depth) const;

    virtual bool can_evaluate_to_function() const;
    virtual bool can_evaluate_to_boolean() const;
};

class Quote : public Expression {
    std::shared_ptr<ast::Element> element;

  public:
    Quote(std::shared_ptr<ast::Element> element);

    static std::unique_ptr<Quote> parse(std::shared_ptr<ast::List> arguments);

    virtual std::shared_ptr<ast::Element> evaluate() const;
    virtual void display(std::ostream& stream, size_t depth) const;

    virtual bool can_evaluate_to_function() const;
    virtual bool can_evaluate_to_boolean() const;
};

class Setq : public Expression {
    std::shared_ptr<ast::Symbol> variable;
    std::unique_ptr<Expression> initializer;

  public:
    Setq(
        std::shared_ptr<ast::Symbol> symbol,
        std::unique_ptr<Expression> expression
    );

    static std::unique_ptr<Setq> parse(std::shared_ptr<ast::List> arguments);

    virtual std::shared_ptr<ast::Element> evaluate() const;
    virtual void display(std::ostream& stream, size_t depth) const;

    virtual bool can_evaluate_to_function() const;
    virtual bool can_evaluate_to_boolean() const;
};

class Cond : public Expression {
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Expression> then;
    std::unique_ptr<Expression> otherwise;

  public:
    Cond(
        std::unique_ptr<Expression> condition,
        std::unique_ptr<Expression> then,
        std::unique_ptr<Expression> otherwise
    );

    static std::unique_ptr<Cond> parse(std::shared_ptr<ast::List> arguments);

    virtual std::shared_ptr<ast::Element> evaluate() const;
    virtual void display(std::ostream& stream, size_t depth) const;

    virtual bool can_evaluate_to_function() const;
    virtual bool can_evaluate_to_boolean() const;
};

class Return : public Expression {
    std::unique_ptr<Expression> expression;

  public:
    Return(std::unique_ptr<Expression> expression);

    static std::unique_ptr<Return> parse(std::shared_ptr<ast::List> arguments);

    virtual std::shared_ptr<ast::Element> evaluate() const;
    virtual void display(std::ostream& stream, size_t depth) const;

    virtual bool can_evaluate_to_function() const;
    virtual bool can_evaluate_to_boolean() const;
};

class Break : public Expression {
    std::unique_ptr<Expression> expression;

  public:
    Break(std::unique_ptr<Expression> expression);

    static std::unique_ptr<Break> parse(std::shared_ptr<ast::List> arguments);

    virtual std::shared_ptr<ast::Element> evaluate() const;
    virtual void display(std::ostream& stream, size_t depth) const;

    virtual bool can_evaluate_to_function() const;
    virtual bool can_evaluate_to_boolean() const;
};

class Call : public Expression {
    std::unique_ptr<Expression> function;
    std::vector<std::unique_ptr<Expression>> arguments;

  public:
    Call(
        std::unique_ptr<Expression> function,
        std::vector<std::unique_ptr<Expression>> arguments
    );

    static std::unique_ptr<Call> parse(ast::Cons const& arguments);

    virtual std::shared_ptr<ast::Element> evaluate() const;
    virtual void display(std::ostream& stream, size_t depth) const;

    virtual bool can_evaluate_to_function() const;
    virtual bool can_evaluate_to_boolean() const;
};

class Program {
    std::vector<std::unique_ptr<Expression>> program;

  public:
    Program(std::vector<std::unique_ptr<Expression>> program);

    static Program
    from_elements(std::vector<std::shared_ptr<ast::Element>> elements);

    std::shared_ptr<ast::Element> evaluate() const;

    void display(std::ostream& stream, size_t depth) const;
    friend std::ostream&
    operator<<(std::ostream& stream, Program const& program);

    bool can_evaluate_to_function() const;
    bool can_evaluate_to_boolean() const;
};

class Func : public Expression {
    std::shared_ptr<ast::Symbol> name;
    std::vector<std::shared_ptr<ast::Symbol>> parameters;
    Program body;

  public:
    Func(
        std::shared_ptr<ast::Symbol> name,
        std::vector<std::shared_ptr<ast::Symbol>> parameters,
        Program body
    );

    static std::unique_ptr<Func> parse(std::shared_ptr<ast::List> arguments);

    virtual std::shared_ptr<ast::Element> evaluate() const;
    virtual void display(std::ostream& stream, size_t depth) const;

    virtual bool can_evaluate_to_function() const;
    virtual bool can_evaluate_to_boolean() const;
};

class Lambda : public Expression {
    std::vector<std::shared_ptr<ast::Symbol>> parameters;
    Program body;

  public:
    Lambda(std::vector<std::shared_ptr<ast::Symbol>> parameters, Program body);

    static std::unique_ptr<Lambda> parse(std::shared_ptr<ast::List> arguments);

    virtual std::shared_ptr<ast::Element> evaluate() const;
    virtual void display(std::ostream& stream, size_t depth) const;

    virtual bool can_evaluate_to_function() const;
    virtual bool can_evaluate_to_boolean() const;
};

class Prog : public Expression {
    std::vector<std::shared_ptr<ast::Symbol>> variables;
    Program body;

  public:
    Prog(std::vector<std::shared_ptr<ast::Symbol>> variables, Program body);

    static std::unique_ptr<Prog> parse(std::shared_ptr<ast::List> arguments);

    virtual std::shared_ptr<ast::Element> evaluate() const;
    virtual void display(std::ostream& stream, size_t depth) const;

    virtual bool can_evaluate_to_function() const;
    virtual bool can_evaluate_to_boolean() const;
};

class While : public Expression {
    std::unique_ptr<Expression> condition;
    Program body;

  public:
    While(std::unique_ptr<Expression> condition, Program body);

    static std::unique_ptr<While> parse(std::shared_ptr<ast::List> arguments);

    virtual std::shared_ptr<ast::Element> evaluate() const;
    virtual void display(std::ostream& stream, size_t depth) const;

    virtual bool can_evaluate_to_function() const;
    virtual bool can_evaluate_to_boolean() const;
};

} // namespace evaluator
