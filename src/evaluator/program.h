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
    virtual void display(std::ostream& stream) const = 0;
};

class Atom : public Expression {
    std::shared_ptr<ast::Element> atom;

  public:
    Atom(std::shared_ptr<ast::Element> atom);

    virtual std::shared_ptr<ast::Element> evaluate() const;
    virtual void display(std::ostream& stream) const;
};

class Quote : public Expression {
    std::shared_ptr<ast::Element> element;

  public:
    Quote(std::shared_ptr<ast::Element> element);

    static std::unique_ptr<Quote> parse(std::shared_ptr<ast::List> arguments);

    virtual std::shared_ptr<ast::Element> evaluate() const;
    virtual void display(std::ostream& stream) const;
};

class Setq : public Expression {
    std::shared_ptr<ast::Symbol> symbol;
    std::unique_ptr<Expression> expression;

  public:
    Setq(std::shared_ptr<ast::Symbol> symbol, std::unique_ptr<Expression> expression);
    
    static std::unique_ptr<Setq> parse(std::shared_ptr<ast::List> arguments);

    virtual std::shared_ptr<ast::Element> evaluate() const;
    virtual void display(std::ostream& stream) const;
};

class Func : public Expression {
    std::shared_ptr<ast::Symbol> name;
    std::shared_ptr<ast::List> arguments;
    std::unique_ptr<Expression> expression;

  public:
    Func(
      std::shared_ptr<ast::Symbol> name,
      std::shared_ptr<ast::List> arguments,
      std::unique_ptr<Expression> expression);
    
    static std::unique_ptr<Func> parse(std::shared_ptr<ast::List> arguments);

    virtual std::shared_ptr<ast::Element> evaluate() const;
    virtual void display(std::ostream& stream) const;
};

class Lambda : public Expression {
    std::shared_ptr<ast::List> arguments;
    std::unique_ptr<Expression> expression;

  public:
    Lambda(
      std::shared_ptr<ast::List> arguments,
      std::unique_ptr<Expression> expression
    );

    static std::unique_ptr<Lambda> parse(std::shared_ptr<ast::List> arguments);

    virtual std::shared_ptr<ast::Element> evaluate() const;
    virtual void display(std::ostream& stream) const;
};

class Prog : public Expression {
    std::shared_ptr<ast::List> arguments;
    std::unique_ptr<Expression> expression;

  public:
    Prog(
      std::shared_ptr<ast::List> arguments,
      std::unique_ptr<Expression> expression
    );

    static std::unique_ptr<Prog> parse(std::shared_ptr<ast::List> arguments);

    virtual std::shared_ptr<ast::Element> evaluate() const;
    virtual void display(std::ostream& stream) const;
};

class Program {
    std::vector<std::unique_ptr<Expression>> program;

  public:
    Program(std::vector<std::unique_ptr<Expression>> program);

    static Program
    from_elements(std::vector<std::unique_ptr<ast::Element>> elements);

    std::shared_ptr<ast::Element> evaluate() const;

    friend std::ostream&
    operator<<(std::ostream& stream, Program const& program);
};

} // namespace evaluator
