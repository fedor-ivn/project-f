#pragma once

#include "../ast/element.h"
#include "../ast/kind.h"
#include "scope.h"
#include <memory>
#include <vector>

namespace evaluator {

class EvaluationContext {
  public:
    GarbageCollector* garbage_collector;
    std::shared_ptr<Scope> scope;

    EvaluationContext(GarbageCollector*, std::shared_ptr<Scope>);
};

class Expression {
  public:
    ast::Span span;

    Expression(ast::Span span);

    static std::unique_ptr<Expression>
    parse(std::shared_ptr<ast::Element> element);

    virtual ~Expression() = default;

    virtual ElementGuard evaluate(EvaluationContext context) const = 0;
    virtual void display(std::ostream& stream, size_t depth) const = 0;

    // Returns in the sense "evaluating this expression will always end up
    // calling `return`"
    virtual bool returns() const = 0;
    virtual bool breaks() const = 0;
    virtual bool diverges() const;
    virtual bool can_evaluate_to(ast::ElementKind kind) const = 0;
    virtual bool can_break_with(ast::ElementKind kind) const = 0;
    virtual void validate_no_free_break() const = 0;
    virtual void validate_no_break_with_value() const = 0;
};

class Parameters {
    ast::Span span;

  public:
    std::vector<std::shared_ptr<ast::Symbol>> parameters;

    Parameters(
        ast::Span span, std::vector<std::shared_ptr<ast::Symbol>> parameters
    );

    static Parameters parse(std::shared_ptr<ast::List> parameter_list);

    void display(std::ostream& stream, size_t depth) const;
};

class Body {
  public:
    std::vector<std::unique_ptr<Expression>> body;

    Body(std::vector<std::unique_ptr<Expression>> body);

    static Body parse(std::shared_ptr<ast::List> unparsed);

    ElementGuard evaluate(EvaluationContext context) const;

    void display(std::ostream& stream, size_t depth) const;

    void validate_no_free_break() const;
    void validate_no_break_with_value() const;
};

class Program {
    Body program;

  public:
    Program(Body program);

    static Program parse(std::vector<std::shared_ptr<ast::Element>> elements);

    ElementGuard evaluate(EvaluationContext context) const;

    void display(std::ostream& stream, size_t depth) const;
    friend std::ostream& operator<<(std::ostream& stream, Program const& self);
};

class Symbol : public Expression {
    std::shared_ptr<ast::Symbol> symbol;

  public:
    Symbol(std::shared_ptr<ast::Symbol> symbol);

    virtual ElementGuard evaluate(EvaluationContext context) const;
    virtual void display(std::ostream& stream, size_t depth) const;

    virtual bool returns() const;
    virtual bool breaks() const;
    virtual bool can_evaluate_to(ast::ElementKind kind) const;
    virtual bool can_break_with(ast::ElementKind kind) const;
    virtual void validate_no_free_break() const;
    virtual void validate_no_break_with_value() const;
};

class Quote : public Expression {
    std::shared_ptr<ast::Element> element;

  public:
    Quote(ast::Span span, std::shared_ptr<ast::Element> element);

    static std::unique_ptr<Quote>
    parse(ast::Span span, std::shared_ptr<ast::List> arguments);

    virtual ElementGuard evaluate(EvaluationContext context) const;
    virtual void display(std::ostream& stream, size_t depth) const;

    virtual bool returns() const;
    virtual bool breaks() const;
    virtual bool can_evaluate_to(ast::ElementKind kind) const;
    virtual bool can_break_with(ast::ElementKind kind) const;
    virtual void validate_no_free_break() const;
    virtual void validate_no_break_with_value() const;
};

class Setq : public Expression {
    std::shared_ptr<ast::Symbol> variable;
    std::unique_ptr<Expression> initializer;

  public:
    Setq(
        ast::Span span,
        std::shared_ptr<ast::Symbol> symbol,
        std::unique_ptr<Expression> expression
    );

    static std::unique_ptr<Setq>
    parse(ast::Span span, std::shared_ptr<ast::List> arguments);

    virtual ElementGuard evaluate(EvaluationContext context) const;
    virtual void display(std::ostream& stream, size_t depth) const;

    virtual bool returns() const;
    virtual bool breaks() const;
    virtual bool can_evaluate_to(ast::ElementKind kind) const;
    virtual bool can_break_with(ast::ElementKind kind) const;
    virtual void validate_no_free_break() const;
    virtual void validate_no_break_with_value() const;
};

class Cond : public Expression {
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Expression> then;
    std::unique_ptr<Expression> otherwise;

  public:
    Cond(
        ast::Span span,
        std::unique_ptr<Expression> condition,
        std::unique_ptr<Expression> then,
        std::unique_ptr<Expression> otherwise
    );

    static std::unique_ptr<Cond>
    parse(ast::Span span, std::shared_ptr<ast::List> arguments);

    virtual ElementGuard evaluate(EvaluationContext context) const;
    virtual void display(std::ostream& stream, size_t depth) const;

    virtual bool returns() const;
    virtual bool breaks() const;
    virtual bool can_evaluate_to(ast::ElementKind kind) const;
    virtual bool can_break_with(ast::ElementKind kind) const;
    virtual void validate_no_free_break() const;
    virtual void validate_no_break_with_value() const;
};

class Return : public Expression {
    std::unique_ptr<Expression> expression;

  public:
    Return(ast::Span span, std::unique_ptr<Expression> expression);

    static std::unique_ptr<Return>
    parse(ast::Span span, std::shared_ptr<ast::List> arguments);

    virtual ElementGuard evaluate(EvaluationContext context) const;
    virtual void display(std::ostream& stream, size_t depth) const;

    virtual bool returns() const;
    virtual bool breaks() const;
    virtual bool can_evaluate_to(ast::ElementKind kind) const;
    virtual bool can_break_with(ast::ElementKind kind) const;
    virtual void validate_no_free_break() const;
    virtual void validate_no_break_with_value() const;
};

class Break : public Expression {
    std::unique_ptr<Expression> expression;

  public:
    Break(ast::Span span, std::unique_ptr<Expression> expression);

    static std::unique_ptr<Break>
    parse(ast::Span span, std::shared_ptr<ast::List> arguments);

    virtual ElementGuard evaluate(EvaluationContext context) const;
    virtual void display(std::ostream& stream, size_t depth) const;

    virtual bool returns() const;
    virtual bool breaks() const;
    virtual bool can_evaluate_to(ast::ElementKind kind) const;
    virtual bool can_break_with(ast::ElementKind kind) const;
    virtual void validate_no_free_break() const;
    virtual void validate_no_break_with_value() const;
};

class Call : public Expression {
    std::unique_ptr<Expression> function;
    std::vector<std::unique_ptr<Expression>> arguments;

  public:
    Call(
        ast::Span span,
        std::unique_ptr<Expression> function,
        std::vector<std::unique_ptr<Expression>> arguments
    );

    static std::unique_ptr<Call> parse(std::shared_ptr<ast::Cons> arguments);

    virtual ElementGuard evaluate(EvaluationContext context) const;
    virtual void display(std::ostream& stream, size_t depth) const;

    virtual bool returns() const;
    virtual bool breaks() const;
    virtual bool can_evaluate_to(ast::ElementKind kind) const;
    virtual bool can_break_with(ast::ElementKind kind) const;
    virtual void validate_no_free_break() const;
    virtual void validate_no_break_with_value() const;
};

class Func : public Expression {
    std::shared_ptr<ast::Symbol> name;
    Parameters parameters;
    std::shared_ptr<Body> body;

  public:
    Func(
        ast::Span span,
        std::shared_ptr<ast::Symbol> name,
        Parameters parameters,
        std::shared_ptr<Body> body
    );

    static std::unique_ptr<Func>
    parse(ast::Span span, std::shared_ptr<ast::List> arguments);

    virtual ElementGuard evaluate(EvaluationContext context) const;
    virtual void display(std::ostream& stream, size_t depth) const;

    virtual bool returns() const;
    virtual bool breaks() const;
    virtual bool can_evaluate_to(ast::ElementKind kind) const;
    virtual bool can_break_with(ast::ElementKind kind) const;
    virtual void validate_no_free_break() const;
    virtual void validate_no_break_with_value() const;
};

class Lambda : public Expression {
    Parameters parameters;
    std::shared_ptr<Body> body;

  public:
    Lambda(ast::Span span, Parameters parameters, std::shared_ptr<Body> body);

    static std::unique_ptr<Lambda>
    parse(ast::Span span, std::shared_ptr<ast::List> arguments);

    virtual ElementGuard evaluate(EvaluationContext context) const;
    virtual void display(std::ostream& stream, size_t depth) const;

    virtual bool returns() const;
    virtual bool breaks() const;
    virtual bool can_evaluate_to(ast::ElementKind kind) const;
    virtual bool can_break_with(ast::ElementKind kind) const;
    virtual void validate_no_free_break() const;
    virtual void validate_no_break_with_value() const;
};

class Prog : public Expression {
    Parameters variables;
    Body body;

  public:
    Prog(ast::Span span, Parameters variables, Body body);

    static std::unique_ptr<Prog>
    parse(ast::Span span, std::shared_ptr<ast::List> arguments);

    virtual ElementGuard evaluate(EvaluationContext context) const;
    virtual void display(std::ostream& stream, size_t depth) const;

    virtual bool returns() const;
    virtual bool breaks() const;
    virtual bool can_evaluate_to(ast::ElementKind kind) const;
    virtual bool can_break_with(ast::ElementKind kind) const;
    virtual void validate_no_free_break() const;
    virtual void validate_no_break_with_value() const;
};

class While : public Expression {
    std::unique_ptr<Expression> condition;
    Body body;

  public:
    While(ast::Span span, std::unique_ptr<Expression> condition, Body body);

    static std::unique_ptr<While>
    parse(ast::Span span, std::shared_ptr<ast::List> arguments);

    virtual ElementGuard evaluate(EvaluationContext context) const;
    virtual void display(std::ostream& stream, size_t depth) const;

    virtual bool returns() const;
    virtual bool breaks() const;
    virtual bool can_evaluate_to(ast::ElementKind kind) const;
    virtual bool can_break_with(ast::ElementKind kind) const;
    virtual void validate_no_free_break() const;
    virtual void validate_no_break_with_value() const;
};

} // namespace evaluator
