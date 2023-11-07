#include "../../utils.h"
#include "../error.h"
#include "../expression.h"

namespace evaluator {

using ast::Boolean;
using ast::Element;
using ast::List;
using ast::Span;
using evaluator::EvaluationError;
using utils::Depth;
using utils::to_cons;

Quote::Quote(Span span, std::shared_ptr<Element> element)
    : Expression(span), element(element) {}

std::unique_ptr<Quote>
Quote::parse(Span span, std::shared_ptr<List> arguments) {
    auto cons = to_cons(arguments);
    if (!cons) {
        throw EvaluationError("`quote` misses an element to quote", span);
    }

    auto element = cons->left;

    cons = to_cons(cons->right);
    if (cons) {
        throw EvaluationError("`quote` has extra arguments", cons->span);
    }

    return std::make_unique<Quote>(Quote(span, element));
}

std::shared_ptr<Element> Quote::evaluate(std::shared_ptr<Scope>) const { return this->element; }

void Quote::display(std::ostream& stream, size_t depth) const {
    stream << "Quote {\n";

    stream << Depth(depth + 1)
           << "element = " << this->element->display_verbose(depth + 1) << '\n';

    stream << Depth(depth + 1) << "span = " << this->span << '\n';

    stream << Depth(depth) << '}';
}

bool Quote::returns() const { return false; }
bool Quote::breaks() const { return false; }
bool Quote::can_evaluate_to(ast::ElementKind kind) const {
    return this->element->kind == kind;
}
bool Quote::can_break_with(ast::ElementKind) const { return false; }
void Quote::validate_no_free_break() const {}
void Quote::validate_no_break_with_value() const {}

} // namespace evaluator
