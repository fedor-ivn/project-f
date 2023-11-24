#include "function.h"

namespace evaluator {

using ast::Element;
using ast::ElementKind;
using ast::Position;
using ast::Span;

CallFrame::CallFrame(
    std::vector<std::shared_ptr<Element>> arguments,
    Span call_site,
    std::shared_ptr<Scope> caller_scope
)
    : arguments(std::move(arguments)), call_site(call_site),
      caller_scope(caller_scope) {}

Function::Function(ast::Span span) : Element(ElementKind::FUNCTION, span) {}

void Function::_display_pretty(std::ostream& stream) const {
    auto name = this->name();
    stream << "#(";
    if (name == "") {
        stream << "lambda";
    } else {
        stream << "func " << name;
    }
    stream << " (";
    this->display_parameters(stream);
    stream << ") ...)";
}

BuiltInFunction::BuiltInFunction()
    : Function(Span(Position(0, 0), Position(0, 0))) {}

void BuiltInFunction::_display_verbose(std::ostream& stream, size_t) const {
    stream << "BuiltInFunction(" << this->name() << ", " << this->span << ")";
}

} // namespace evaluator
