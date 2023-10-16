#pragma once

#include <cmath>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>

#include "span.h"

namespace ast {

class DisplayVerbose;

class Cons;

class Element {
  public:
    Span span;

    Element(Span span);
    virtual ~Element() = default;

    bool is_null() const;
    std::optional<int64_t> to_integer() const;
    std::optional<double> to_real() const;
    std::optional<bool> to_boolean() const;
    std::optional<std::string_view> to_symbol() const;
    std::optional<Cons> to_cons() const;

    DisplayVerbose display_verbose();

  private:
    virtual void _display_verbose(std::ostream& stream, size_t depth) const = 0;

    friend std::ostream&
    operator<<(std::ostream& stream, const DisplayVerbose& self);
    friend Cons;
};

class Null : public Element {
  public:
    using Element::Element;

  private:
    virtual void _display_verbose(std::ostream& stream, size_t depth) const;
};

class Integer : public Element {
  public:
    int64_t value;

    Integer(int64_t value, Span span);

  private:
    virtual void _display_verbose(std::ostream& stream, size_t depth) const;
};

class Real : public Element {
  public:
    double value;

    Real(double value, Span span);

  private:
    virtual void _display_verbose(std::ostream& stream, size_t depth) const;
};

class Boolean : public Element {
  public:
    bool value;

    Boolean(bool value, Span span);

  private:
    virtual void _display_verbose(std::ostream& stream, size_t depth) const;
};

class Symbol : public Element {
  public:
    std::string value;

    Symbol(std::string value, Span span);

  private:
    virtual void _display_verbose(std::ostream& stream, size_t depth) const;
};

class Cons : public Element {
  public:
    std::shared_ptr<Element> left;
    std::shared_ptr<Element> right;

    Cons(
        std::shared_ptr<Element> left, std::shared_ptr<Element> right, Span span
    );

  private:
    virtual void _display_verbose(std::ostream& stream, size_t depth) const;
};

class DisplayVerbose {
    Element* element;

    DisplayVerbose(Element* element);

    friend DisplayVerbose Element::display_verbose();

    friend std::ostream&
    operator<<(std::ostream& stream, const DisplayVerbose& self);
};

} // namespace ast
