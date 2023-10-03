#include <memory>

#include "element.h"

using namespace element;

Element::Element(Span span) : span(span) {}

bool Element::is_null() const {
    return dynamic_cast<const Null*>(this) != nullptr;
}

std::optional<int64_t> Element::to_integer() const {
    if (auto element = dynamic_cast<const Integer*>(this)) {
        return std::optional<int64_t>(element->value);
    }
    return std::nullopt;
}

std::optional<double> Element::to_real() const {
    if (auto element = dynamic_cast<const Real*>(this)) {
        return std::optional<double>(element->value);
    }
    return std::nullopt;
}

std::optional<bool> Element::to_boolean() const {
    if (auto element = dynamic_cast<const Boolean*>(this)) {
        return std::optional<bool>(element->value);
    }
    return std::nullopt;
}

std::optional<std::string_view> Element::to_symbol() const {
    if (auto element = dynamic_cast<const Symbol*>(this)) {
        return std::optional<std::string_view>(
            std::string_view(element->value));
    }
    return std::nullopt;
}

Integer::Integer(int64_t value, Span span) : Element(span), value(value) {}

Real::Real(double value, Span span) : Element(span), value(value) {}

Boolean::Boolean(bool value, Span span) : Element(span), value(value) {}

Symbol::Symbol(std::string value, Span span) : Element(span), value(value) {}

Cons::Cons(std::shared_ptr<Element> left, Span span)
    : Element(span), left(left) {
    this->right = std::make_shared<Null>(Null(span));
}

Cons::Cons(std::shared_ptr<Element> left, std::shared_ptr<Element> right,
           Span span)
    : Element(span), left(left), right(right) {}

std::ostream& element::operator<<(std::ostream& stream,
                                  const Element& element) {
    if (element.is_null()) {
        stream << "Null";
    } else if (auto value = element.to_integer()) {
        stream << "Integer(" << *value << ")";
    } else if (auto value = element.to_real()) {
        stream << "Real(" << *value << ")";
    } else if (auto value = element.to_boolean()) {
        stream << "Boolean(" << *value << ")";
    } else if (auto value = element.to_symbol()) {
        stream << "Symbol(" << *value << ")";
    } else if (auto cons = dynamic_cast<const Cons*>(&element)) {
        stream << "Cons(" << *cons->left << ", " << *cons->right << ")";
    }

    stream << " at " << element.span;

    return stream;
}
