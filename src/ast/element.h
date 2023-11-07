#pragma once

#include <cmath>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>

#include "kind.h"
#include "span.h"

namespace ast {

class DisplayVerbose;
class DisplayPretty;

class Cons;

class Element {
  public:
    Span span;
    ElementKind kind;

    Element(ElementKind kind, Span span);
    virtual ~Element() = default;

    DisplayVerbose display_verbose(size_t depth = 0);
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
    Null(Span span);

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
    size_t depth;

    DisplayVerbose(Element* element, size_t depth);

    friend DisplayVerbose Element::display_verbose(size_t depth);

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
