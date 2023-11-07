#include "../expression.h"

namespace evaluator {

using ast::Element;

Symbol::Symbol(std::shared_ptr<ast::Symbol> symbol)
    : Expression(symbol->span), symbol(symbol) {}

std::shared_ptr<Element> Symbol::evaluate(std::shared_ptr<Scope> parent) const {
    return parent->lookup(*this->symbol);
}

void Symbol::display(std::ostream& stream, size_t depth) const {
    stream << this->symbol->display_verbose(depth);
}

bool Symbol::can_evaluate_to_function() const { return true; }
bool Symbol::can_evaluate_to_boolean() const { return true; }
void Symbol::validate_no_free_break() const {}
void Symbol::validate_no_break_with_value() const {}

} // namespace evaluator
