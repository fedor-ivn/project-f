#pragma once

#include <cmath>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>

#include "span.h"

namespace element {

class Element {
  public:
    Span span;

    Element(Span span);
    
    bool is_null() const;
    std::optional<int64_t> to_integer() const;
    std::optional<double> to_real() const;
    std::optional<bool> to_boolean() const;
    std::optional<std::string_view> to_symbol() const;
    // std::optional<Cons> to_list() const;

    virtual ~Element() = default;
    friend std::ostream& operator<<(std::ostream& stream,
                                    const Element& element);
};

class Null : public Element {
    using Element::Element;
};

class Integer : public Element {
  public:
    int64_t value;

    Integer(int64_t value, Span span);
};

class Real : public Element {
  public:
    double value;

    Real(double value, Span span);
};

class Boolean : public Element {
  public:
    bool value;

    Boolean(bool value, Span span);
};

class Symbol : public Element {
  public:
    std::string value;

    Symbol(std::string value, Span span);
};

class Cons : public Element {
  public:
    std::shared_ptr<Element> left;
    std::shared_ptr<Element> right;

    Cons(std::shared_ptr<Element> left, Span span);

    Cons(std::shared_ptr<Element> left, std::shared_ptr<Element> right,
         Span span);
};

} // namespace element
