#include <memory>

#include "element.h"

namespace ast {

class Depth {
  public:
    size_t depth;

    Depth(size_t depth) : depth(depth) {}

    friend std::ostream& operator<<(std::ostream& stream, const Depth& self) {
        for (int i = 0; i < self.depth * 2; ++i) {
            stream << ' ';
        }
        return stream;
    }
};

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
        return std::optional<std::string_view>(std::string_view(element->value)
        );
    }
    return std::nullopt;
}

std::optional<Cons> Element::to_cons() const {
    if (auto element = dynamic_cast<const Cons*>(this)) {
        return std::optional<Cons>(*element);
    }
    return std::nullopt;
}

void Null::_display_verbose(std::ostream& stream, size_t depth) const {
    stream << Depth(depth) << "Null(" << this->span << ")";
}

DisplayVerbose Element::display_verbose() { return DisplayVerbose(this); }

Integer::Integer(int64_t value, Span span) : Element(span), value(value) {}

void Integer::_display_verbose(std::ostream& stream, size_t depth) const {
    stream << Depth(depth) << "Integer(" << this->value << ", " << this->span
           << ")";
}

Real::Real(double value, Span span) : Element(span), value(value) {}

void Real::_display_verbose(std::ostream& stream, size_t depth) const {
    stream << Depth(depth) << "Real(" << this->value << ", " << this->span
           << ")";
}

Boolean::Boolean(bool value, Span span) : Element(span), value(value) {}

void Boolean::_display_verbose(std::ostream& stream, size_t depth) const {
    auto value = this->value ? "true" : "false";
    stream << Depth(depth) << "Boolean(" << value << ", " << this->span << ")";
}

Symbol::Symbol(std::string value, Span span) : Element(span), value(value) {}

void Symbol::_display_verbose(std::ostream& stream, size_t depth) const {
    stream << Depth(depth) << "Symbol(" << this->value << ", " << this->span
           << ")";
}

Cons::Cons(
    std::shared_ptr<Element> left, std::shared_ptr<Element> right, Span span
)
    : Element(span), left(left), right(right) {}

void Cons::_display_verbose(std::ostream& stream, size_t depth) const {
    stream << Depth(depth) << "Cons(\n";

    this->left->_display_verbose(stream, depth + 1);
    stream << ",\n";

    this->right->_display_verbose(stream, depth + 1);
    stream << ",\n";

    stream << Depth(depth + 1) << this->span << '\n';
    stream << Depth(depth) << ')';
}

DisplayVerbose::DisplayVerbose(Element* element) : element(element) {}

std::ostream& operator<<(std::ostream& stream, const DisplayVerbose& self) {
    self.element->_display_verbose(stream, 0);
    return stream;
}

} // namespace ast
