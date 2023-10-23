#include <iomanip>
#include <memory>

#include "element.h"

namespace ast {

class Depth {
  public:
    size_t depth;

    Depth(size_t depth) : depth(depth) {}

    friend std::ostream& operator<<(std::ostream& stream, const Depth& self) {
        for (size_t i = 0; i < self.depth; ++i) {
            stream << "  ";
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

DisplayVerbose Element::display_verbose() { return DisplayVerbose(this); }
DisplayPretty Element::display_pretty() { return DisplayPretty(this); }

Integer::Integer(int64_t value, Span span) : Element(span), value(value) {}

void Integer::_display_verbose(std::ostream& stream, size_t depth) const {
    stream << Depth(depth) << "Integer(" << this->value << ", " << this->span
           << ")";
}

void Integer::_display_pretty(std::ostream& stream) const {
    stream << this->value;
}

Real::Real(double value, Span span) : Element(span), value(value) {}

void Real::_display_verbose(std::ostream& stream, size_t depth) const {
    stream << Depth(depth) << "Real(" << this->value << ", " << this->span
           << ")";
}

void Real::_display_pretty(std::ostream& stream) const {
    // Precision of 16 digits is not actually always enough to read the value
    // back without loss. Precision of 17 digits guarantees that, but it gives
    // obscure printing of many values (i.e. 0.1 as 0.10000000000000001).
    //
    // This also may use scientific notation. It'd be better to avoid it since
    // our language doesn't support it, but if we use fixed notation, then we
    // get trailing zeros.
    //
    // With `std::format`, it seems to be possible to format doubles just like
    // any sane language does. However, it's only supported in very recent
    // compilers (GCC added it only in version 13, while Ubuntu 22.04 has only
    // 11.4). Printing the double on our own is way too much. So I guess we'll
    // have to stick to this suboptimal solution.
    //
    // https://www.zverovich.net/2023/06/04/printing-double.html
    stream << std::setprecision(16) << this->value;
    if (this->value == trunc(this->value)) {
        stream << ".0";
    }
}

Boolean::Boolean(bool value, Span span) : Element(span), value(value) {}

void Boolean::_display_verbose(std::ostream& stream, size_t depth) const {
    auto value = this->value ? "true" : "false";
    stream << Depth(depth) << "Boolean(" << value << ", " << this->span << ")";
}

void Boolean::_display_pretty(std::ostream& stream) const {
    auto value = this->value ? "true" : "false";
    stream << value;
}

Symbol::Symbol(std::string value, Span span) : Element(span), value(value) {}

void Symbol::_display_verbose(std::ostream& stream, size_t depth) const {
    stream << Depth(depth) << "Symbol(" << this->value << ", " << this->span
           << ")";
}

void Symbol::_display_pretty(std::ostream& stream) const {
    stream << this->value;
}

void Null::_display_verbose(std::ostream& stream, size_t depth) const {
    stream << Depth(depth) << "Null(" << this->span << ")";
}

void Null::_display_pretty(std::ostream& stream) const { stream << "null"; }

Cons::Cons(
    std::shared_ptr<Element> left, std::shared_ptr<List> right, Span span
)
    : List(span), left(left), right(right) {}

void Cons::_display_verbose(std::ostream& stream, size_t depth) const {
    stream << Depth(depth) << "Cons(\n";

    this->left->_display_verbose(stream, depth + 1);
    stream << ",\n";

    this->right->_display_verbose(stream, depth + 1);
    stream << ",\n";

    stream << Depth(depth + 1) << this->span << '\n';
    stream << Depth(depth) << ')';
}

void Cons::_display_pretty(std::ostream& stream) const {
    stream << '(';
    this->left->_display_pretty(stream);

    auto current = this;
    while (auto next = current->right->to_cons()) {
        stream << ' ';
        next->left->_display_pretty(stream);
        current = &*next;
    }

    stream << ')';
}

DisplayVerbose::DisplayVerbose(Element* element) : element(element) {}

std::ostream& operator<<(std::ostream& stream, const DisplayVerbose& self) {
    self.element->_display_verbose(stream, 0);
    return stream;
}

DisplayPretty::DisplayPretty(Element* element) : element(element) {}

std::ostream& operator<<(std::ostream& stream, const DisplayPretty& self) {
    self.element->_display_pretty(stream);
    return stream;
}

} // namespace ast
