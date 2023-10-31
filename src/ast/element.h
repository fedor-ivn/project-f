#pragma once

#include <cmath>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>

#include "span.h"

namespace ast {

class DisplayVerbose;
class DisplayPretty;

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
    DisplayPretty display_pretty();

  private:
    virtual void _display_verbose(std::ostream& stream, size_t depth) const = 0;
    virtual void _display_pretty(std::ostream& stream) const = 0;

    friend std::ostream&
    operator<<(std::ostream& stream, DisplayVerbose const& self);
    friend std::ostream&
    operator<<(std::ostream& stream, DisplayPretty const& self);
    friend Cons;
};

class Integer : public Element {
  public:
    int64_t value;

    Integer(int64_t value, Span span);

  private:
    virtual void _display_verbose(std::ostream& stream, size_t depth) const;
    virtual void _display_pretty(std::ostream& stream) const;
};

class Real : public Element {
  public:
    double value;

    Real(double value, Span span);

  private:
    virtual void _display_verbose(std::ostream& stream, size_t depth) const;
    virtual void _display_pretty(std::ostream& stream) const;
};

class Boolean : public Element {
  public:
    bool value;

    Boolean(bool value, Span span);

  private:
    virtual void _display_verbose(std::ostream& stream, size_t depth) const;
    virtual void _display_pretty(std::ostream& stream) const;
};

class Symbol : public Element {
  public:
    std::string value;

    Symbol(std::string value, Span span);

  private:
    virtual void _display_verbose(std::ostream& stream, size_t depth) const;
    virtual void _display_pretty(std::ostream& stream) const;
};

class List : public Element {
    using Element::Element;
};

class Null : public List {
  public:
    using List::List;

  private:
    virtual void _display_verbose(std::ostream& stream, size_t depth) const;
    virtual void _display_pretty(std::ostream& stream) const;
};

class Cons : public List {
  public:
    std::shared_ptr<Element> left;
    std::shared_ptr<List> right;

    Cons(std::shared_ptr<Element> left, std::shared_ptr<List> right, Span span);

  private:
    virtual void _display_verbose(std::ostream& stream, size_t depth) const;
    virtual void _display_pretty(std::ostream& stream) const;
};

class DisplayVerbose {
    Element* element;

    DisplayVerbose(Element* element);

    friend DisplayVerbose Element::display_verbose();

    friend std::ostream&
    operator<<(std::ostream& stream, DisplayVerbose const& self);
};

class DisplayPretty {
    Element* element;

    DisplayPretty(Element* element);

    friend DisplayPretty Element::display_pretty();

    friend std::ostream&
    operator<<(std::ostream& stream, DisplayPretty const& self);
};

} // namespace ast
