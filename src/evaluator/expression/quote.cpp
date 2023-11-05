#include "../../utils.h"
#include "../error.h"
#include "../expression.h"

namespace evaluator {

using ast::Element;
using ast::List;
using evaluator::EvaluationError;
using utils::Depth;

Quote::Quote(std::shared_ptr<Element> element)
    : Expression(), element(element) {}

std::unique_ptr<Quote> Quote::parse(std::shared_ptr<List> arguments) {
    auto form_span = arguments->span;
    auto cons = arguments->to_cons();
    if (!cons) {
        throw EvaluationError("`quote` is empty", form_span);
    }

    auto element = cons->left;

    cons = cons->right->to_cons();
    if (cons) {
        throw EvaluationError("`quote` has extra arguments", cons->span);
    }

    return std::make_unique<Quote>(Quote(element));
}

std::shared_ptr<Element> Quote::evaluate() const { return this->element; }

void Quote::display(std::ostream& stream, size_t depth) const {
    stream << "Quote {\n";
    stream << Depth(depth + 1)
           << "element = " << this->element->display_verbose(depth + 1) << '\n';
    stream << Depth(depth) << '}';
}

bool Quote::can_evaluate_to_function() const { return false; }
bool Quote::can_evaluate_to_boolean() const {
    return this->element->to_boolean() != std::nullopt;
}

} // namespace evaluator
