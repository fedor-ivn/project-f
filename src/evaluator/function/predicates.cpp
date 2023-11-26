#include "../error.h"
#include "../function.h"

namespace evaluator {

using ast::Boolean;
using ast::ElementKind;

ElementGuard is_element_kind(
    CallFrame const& frame, ElementKind kind, std::string_view name
) {
    if (frame.arguments.size() != 1) {
        throw EvaluationError(
            "`" + std::string(name) + "` expects 1 argument, received " +
                std::to_string(frame.arguments.size()),
            frame.call_site
        );
    }

    auto element = frame.arguments[0];
    bool result = element->kind == kind;
    return frame.context.garbage_collector->temporary(
        std::make_shared<Boolean>(result, frame.call_site)
    );
}

ElementGuard IsIntFunction::call(CallFrame frame) const {
    return is_element_kind(frame, ElementKind::INTEGER, this->name());
}
std::string_view IsIntFunction::name() const { return "isint"; }
void IsIntFunction::display_parameters(std::ostream& stream) const {
    stream << "integer";
}

ElementGuard IsRealFunction::call(CallFrame frame) const {
    return is_element_kind(frame, ElementKind::REAL, this->name());
}
std::string_view IsRealFunction::name() const { return "isreal"; }
void IsRealFunction::display_parameters(std::ostream& stream) const {
    stream << "real";
}

ElementGuard IsBoolFunction::call(CallFrame frame) const {
    return is_element_kind(frame, ElementKind::BOOLEAN, this->name());
}
std::string_view IsBoolFunction::name() const { return "isbool"; }
void IsBoolFunction::display_parameters(std::ostream& stream) const {
    stream << "boolean";
}

ElementGuard IsNullFunction::call(CallFrame frame) const {
    return is_element_kind(frame, ElementKind::NULL_, this->name());
}
std::string_view IsNullFunction::name() const { return "isnull"; }
void IsNullFunction::display_parameters(std::ostream& stream) const {
    stream << "null";
}

ElementGuard IsAtomFunction::call(CallFrame frame) const {
    return is_element_kind(frame, ElementKind::SYMBOL, this->name());
}
std::string_view IsAtomFunction::name() const { return "isatom"; }
void IsAtomFunction::display_parameters(std::ostream& stream) const {
    stream << "symbol";
}

ElementGuard IsListFunction::call(CallFrame frame) const {
    if (frame.arguments.size() != 1) {
        throw EvaluationError(
            "`islist` expects 1 argument, received " +
                std::to_string(frame.arguments.size()),
            frame.call_site
        );
    }

    auto element = frame.arguments[0];
    bool result = element->kind == ElementKind::CONS ||
                  element->kind == ElementKind::NULL_;
    return frame.context.garbage_collector->temporary(
        std::make_shared<Boolean>(result, frame.call_site)
    );
}

std::string_view IsListFunction::name() const { return "islist"; }
void IsListFunction::display_parameters(std::ostream& stream) const {
    stream << "list";
}

ElementGuard IsFuncFunction::call(CallFrame frame) const {
    return is_element_kind(frame, ElementKind::FUNCTION, this->name());
}
std::string_view IsFuncFunction::name() const { return "isfunc"; }
void IsFuncFunction::display_parameters(std::ostream& stream) const {
    stream << "function";
}

} // namespace evaluator
