#include <vector>

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
