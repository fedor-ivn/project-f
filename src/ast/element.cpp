#include <iomanip>
#include <memory>

#include "../utils.h"
#include "element.h"

namespace ast {

using utils::Depth;
using utils::to_cons;

Element::Element(ElementKind kind, Span span) : span(span), kind(kind) {}

DisplayVerbose Element::display_verbose(size_t depth) {
    return DisplayVerbose(this, depth);
}
DisplayPretty Element::display_pretty() { return DisplayPretty(this); }

Integer::Integer(int64_t value, Span span)
    : Element(ElementKind::INTEGER, span), value(value) {}

void Integer::_display_verbose(std::ostream& stream, size_t) const {
    stream << "Integer(" << this->value << ", " << this->span << ")";
}

void Integer::_display_pretty(std::ostream& stream) const {
    stream << this->value;
}

Real::Real(double value, Span span)
    : Element(ElementKind::REAL, span), value(value) {}

void Real::_display_verbose(std::ostream& stream, size_t) const {
    stream << "Real(" << this->value << ", " << this->span << ")";
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

Boolean::Boolean(bool value, Span span)
    : Element(ElementKind::BOOLEAN, span), value(value) {}

void Boolean::_display_verbose(std::ostream& stream, size_t) const {
    auto value = this->value ? "true" : "false";
    stream << "Boolean(" << value << ", " << this->span << ")";
}

void Boolean::_display_pretty(std::ostream& stream) const {
    auto value = this->value ? "true" : "false";
    stream << value;
}

Symbol::Symbol(std::string value, Span span)
    : Element(ElementKind::SYMBOL, span), value(value) {}

void Symbol::_display_verbose(std::ostream& stream, size_t) const {
    stream << "Symbol(" << this->value << ", " << this->span << ")";
}

void Symbol::_display_pretty(std::ostream& stream) const {
    stream << this->value;
}

Null::Null(Span span) : List(ElementKind::NULL_, span) {}

void Null::_display_verbose(std::ostream& stream, size_t) const {
    stream << "Null(" << this->span << ")";
}

void Null::_display_pretty(std::ostream& stream) const { stream << "null"; }

Cons::Cons(
    std::shared_ptr<Element> left, std::shared_ptr<List> right, Span span
)
    : List(ElementKind::CONS, span), left(left), right(right) {}

void Cons::_display_verbose(std::ostream& stream, size_t depth) const {
    stream << "Cons(\n";

    stream << Depth(depth + 1);
    this->left->_display_verbose(stream, depth + 1);
    stream << ",\n";

    stream << Depth(depth + 1);
    this->right->_display_verbose(stream, depth + 1);
    stream << ",\n";

    stream << Depth(depth + 1) << this->span << '\n';
    stream << Depth(depth) << ')';
}

void Cons::_display_pretty(std::ostream& stream) const {
    stream << '(';
    this->left->_display_pretty(stream);

    auto current = this;
    while (auto next = to_cons(current->right)) {
        stream << ' ';
        next->left->_display_pretty(stream);
        current = &*next;
    }

    stream << ')';
}

DisplayVerbose::DisplayVerbose(Element* element, size_t depth)
    : element(element), depth(depth) {}

std::ostream& operator<<(std::ostream& stream, DisplayVerbose const& self) {
    self.element->_display_verbose(stream, self.depth);
    return stream;
}

DisplayPretty::DisplayPretty(Element* element) : element(element) {}

std::ostream& operator<<(std::ostream& stream, DisplayPretty const& self) {
    self.element->_display_pretty(stream);
    return stream;
}

} // namespace ast
