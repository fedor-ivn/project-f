#include "../error.h"
#include "../function.h"

namespace evaluator {

using ast::Cons;
using ast::Element;
using ast::List;
using ast::Null;

ElementGuard HeadFunction::call(CallFrame frame) const {
    if (frame.arguments.size() != 1) {
        throw EvaluationError(
            "`head` expects 1 argument, received " +
                std::to_string(frame.arguments.size()),
            frame.call_site
        );
    }
    auto list = frame.arguments[0];

    if (auto cons = std::dynamic_pointer_cast<Cons>(list)) {
        return frame.context.garbage_collector->temporary(cons->left);
    }

    if (auto null = std::dynamic_pointer_cast<Null>(list)) {
        return frame.context.garbage_collector->temporary(null);
    }

    throw EvaluationError(
        "`cons` expects the first argument to be a list", frame.call_site
    );
}

std::string_view HeadFunction::name() const { return "head"; }
void HeadFunction::display_parameters(std::ostream& stream) const {
    stream << "list";
}

ElementGuard TailFunction::call(CallFrame frame) const {
    if (frame.arguments.size() != 1) {
        throw EvaluationError(
            "`tail` expects 1 argument, received " +
                std::to_string(frame.arguments.size()),
            frame.call_site
        );
    }
    auto list = frame.arguments[0];

    if (auto cons = std::dynamic_pointer_cast<Cons>(list)) {
        return frame.context.garbage_collector->temporary(cons->right);
    }

    if (auto null = std::dynamic_pointer_cast<Null>(list)) {
        return frame.context.garbage_collector->temporary(null);
    }

    throw EvaluationError(
        "`tail` expects the first argument to be a list", frame.call_site
    );
}

std::string_view TailFunction::name() const { return "tail"; }
void TailFunction::display_parameters(std::ostream& stream) const {
    stream << "list";
}

ElementGuard ConsFunction::call(CallFrame frame) const {
    if (frame.arguments.size() != 2) {
        throw EvaluationError(
            "`cons` expects 2 arguments, received " +
                std::to_string(frame.arguments.size()),
            frame.call_site
        );
    }

    auto left = frame.arguments[0];
    auto right = std::dynamic_pointer_cast<List>(frame.arguments[1]);
    if (!right) {
        throw EvaluationError(
            "`cons` expects the second argument to be a list", frame.call_site
        );
    }

    return frame.context.garbage_collector->temporary(
        std::make_unique<Cons>(left, right, frame.call_site)
    );
}

std::string_view ConsFunction::name() const { return "cons"; }
void ConsFunction::display_parameters(std::ostream& stream) const {
    stream << "head tail";
}

} // namespace evaluator
