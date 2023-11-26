#include "../ast/element.h"
#include "expression.h"
#include "scope.h"

namespace evaluator {

class CallFrame {
  public:
    std::vector<std::shared_ptr<ast::Element>> arguments;
    ast::Span call_site;
    EvaluationContext context;

    CallFrame(
        std::vector<std::shared_ptr<ast::Element>> arguments,
        ast::Span call_site,
        EvaluationContext context
    );
};

class Function : public ast::Element {
  public:
    Function(ast::Span span);

    virtual ElementGuard call(CallFrame frame) const = 0;

  protected:
    virtual std::string_view name() const = 0;
    virtual void display_parameters(std::ostream& stream) const = 0;
    void _display_pretty(std::ostream& stream) const;
};

class UserDefinedFunction : public Function {
  public:
    UserDefinedFunction(
        ast::Span span,
        Parameters parameters,
        std::shared_ptr<Body> body,
        std::shared_ptr<Scope> scope
    );

    virtual ElementGuard call(CallFrame frame) const;

  protected:
    virtual void display_parameters(std::ostream& stream) const;
    virtual void _display_verbose(std::ostream& stream, size_t depth) const = 0;

  private:
    Parameters parameters;
    std::shared_ptr<Body> body;

    std::shared_ptr<Scope> scope;
};

class FuncFunction : public UserDefinedFunction {
  public:
    FuncFunction(
        ast::Span span,
        std::string name,
        Parameters parameters,
        std::shared_ptr<Body> body,
        std::shared_ptr<Scope> scope
    );

  protected:
    virtual void _display_verbose(std::ostream& stream, size_t depth) const;
    virtual std::string_view name() const;

  private:
    std::string _name;
};

class LambdaFunction : public UserDefinedFunction {
  public:
    LambdaFunction(
        ast::Span span,
        Parameters parameters,
        std::shared_ptr<Body> body,
        std::shared_ptr<Scope> scope
    );

  protected:
    virtual void _display_verbose(std::ostream& stream, size_t depth) const;
    virtual std::string_view name() const;
};

class BuiltInFunction : public Function {
  public:
    BuiltInFunction();

  private:
    virtual void _display_verbose(std::ostream& stream, size_t depth) const;
};

class PlusFunction : public BuiltInFunction {
  public:
    using BuiltInFunction::BuiltInFunction;
    virtual ElementGuard call(CallFrame frame) const;

  private:
    virtual std::string_view name() const;
    virtual void display_parameters(std::ostream& stream) const;
};

class TimesFunction : public BuiltInFunction {
  public:
    using BuiltInFunction::BuiltInFunction;
    virtual ElementGuard call(CallFrame frame) const;

  private:
    virtual std::string_view name() const;
    virtual void display_parameters(std::ostream& stream) const;
};

class MinusFunction : public BuiltInFunction {
  public:
    using BuiltInFunction::BuiltInFunction;
    virtual ElementGuard call(CallFrame frame) const;

  private:
    virtual std::string_view name() const;
    virtual void display_parameters(std::ostream& stream) const;
};

class DivideFunction : public BuiltInFunction {
  public:
    using BuiltInFunction::BuiltInFunction;
    virtual ElementGuard call(CallFrame frame) const;

  private:
    virtual std::string_view name() const;
    virtual void display_parameters(std::ostream& stream) const;
};

class HeadFunction : public BuiltInFunction {
  public:
    using BuiltInFunction::BuiltInFunction;
    virtual ElementGuard call(CallFrame frame) const;

  protected:
    virtual std::string_view name() const;
    virtual void display_parameters(std::ostream& stream) const;
};

class TailFunction : public BuiltInFunction {
  public:
    using BuiltInFunction::BuiltInFunction;
    virtual ElementGuard call(CallFrame frame) const;

  protected:
    virtual std::string_view name() const;
    virtual void display_parameters(std::ostream& stream) const;
};

class ConsFunction : public BuiltInFunction {
  public:
    using BuiltInFunction::BuiltInFunction;
    virtual ElementGuard call(CallFrame frame) const;

  protected:
    virtual std::string_view name() const;
    virtual void display_parameters(std::ostream& stream) const;
};

class EvalFunction : public BuiltInFunction {
  public:
    using BuiltInFunction::BuiltInFunction;
    virtual ElementGuard call(CallFrame frame) const;

  protected:
    virtual std::string_view name() const;
    virtual void display_parameters(std::ostream& stream) const;
};

class EqualFunction : public BuiltInFunction {
  public:
    using BuiltInFunction::BuiltInFunction;
    virtual ElementGuard call(CallFrame frame) const;

  protected:
    virtual std::string_view name() const;
    virtual void display_parameters(std::ostream& stream) const;
};

class NonequalFunction : public BuiltInFunction {
  public:
    using BuiltInFunction::BuiltInFunction;
    virtual ElementGuard call(CallFrame frame) const;

  protected:
    virtual std::string_view name() const;
    virtual void display_parameters(std::ostream& stream) const;
};

class LessFunction : public BuiltInFunction {
  public:
    using BuiltInFunction::BuiltInFunction;
    virtual ElementGuard call(CallFrame frame) const;

  protected:
    virtual std::string_view name() const;
    virtual void display_parameters(std::ostream& stream) const;
};

class LesseqFunction : public BuiltInFunction {
  public:
    using BuiltInFunction::BuiltInFunction;
    virtual ElementGuard call(CallFrame frame) const;

  protected:
    virtual std::string_view name() const;
    virtual void display_parameters(std::ostream& stream) const;
};

class GreaterFunction : public BuiltInFunction {
  public:
    using BuiltInFunction::BuiltInFunction;
    virtual ElementGuard call(CallFrame frame) const;

  protected:
    virtual std::string_view name() const;
    virtual void display_parameters(std::ostream& stream) const;
};

class GreatereqFunction : public BuiltInFunction {
  public:
    using BuiltInFunction::BuiltInFunction;
    virtual ElementGuard call(CallFrame frame) const;

  protected:
    virtual std::string_view name() const;
    virtual void display_parameters(std::ostream& stream) const;
};

class IsIntFunction : public BuiltInFunction {
  public:
    using BuiltInFunction::BuiltInFunction;
    virtual ElementGuard call(CallFrame frame) const;

    virtual std::string_view name() const;
    virtual void display_parameters(std::ostream& stream) const;
};

class IsRealFunction : public BuiltInFunction {
  public:
    using BuiltInFunction::BuiltInFunction;
    virtual ElementGuard call(CallFrame frame) const;

  protected:
    virtual std::string_view name() const;
    virtual void display_parameters(std::ostream& stream) const;
};

class IsBoolFunction : public BuiltInFunction {
  public:
    using BuiltInFunction::BuiltInFunction;
    virtual ElementGuard call(CallFrame frame) const;

  protected:
    virtual std::string_view name() const;
    virtual void display_parameters(std::ostream& stream) const;
};

class IsNullFunction : public BuiltInFunction {
  public:
    using BuiltInFunction::BuiltInFunction;
    virtual ElementGuard call(CallFrame frame) const;

  protected:
    virtual std::string_view name() const;
    virtual void display_parameters(std::ostream& stream) const;
};

class IsAtomFunction : public BuiltInFunction {
  public:
    using BuiltInFunction::BuiltInFunction;
    virtual ElementGuard call(CallFrame frame) const;

  protected:
    virtual std::string_view name() const;
    virtual void display_parameters(std::ostream& stream) const;
};

class IsListFunction : public BuiltInFunction {
  public:
    using BuiltInFunction::BuiltInFunction;
    virtual ElementGuard call(CallFrame frame) const;

  protected:
    virtual std::string_view name() const;
    virtual void display_parameters(std::ostream& stream) const;
};

class IsFuncFunction : public BuiltInFunction {
  public:
    using BuiltInFunction::BuiltInFunction;
    virtual ElementGuard call(CallFrame frame) const;

  protected:
    virtual std::string_view name() const;
    virtual void display_parameters(std::ostream& stream) const;
};

class AndFunction : public BuiltInFunction {
  public:
    using BuiltInFunction::BuiltInFunction;
    virtual ElementGuard call(CallFrame frame) const;

  protected:
    virtual std::string_view name() const;
    virtual void display_parameters(std::ostream& stream) const;
};

class OrFunction : public BuiltInFunction {
  public:
    using BuiltInFunction::BuiltInFunction;
    virtual ElementGuard call(CallFrame frame) const;

  protected:
    virtual std::string_view name() const;
    virtual void display_parameters(std::ostream& stream) const;
};

class XorFunction : public BuiltInFunction {
  public:
    using BuiltInFunction::BuiltInFunction;
    virtual ElementGuard call(CallFrame frame) const;

  protected:
    virtual std::string_view name() const;
    virtual void display_parameters(std::ostream& stream) const;
};

class NotFunction : public BuiltInFunction {
  public:
    using BuiltInFunction::BuiltInFunction;
    virtual ElementGuard call(CallFrame frame) const;

  protected:
    virtual std::string_view name() const;
    virtual void display_parameters(std::ostream& stream) const;
};

} // namespace evaluator
