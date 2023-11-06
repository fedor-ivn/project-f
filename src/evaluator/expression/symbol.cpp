#include "../expression.h"

namespace evaluator {

using ast::Element;

Symbol::Symbol(std::shared_ptr<ast::Symbol> symbol)
    : Expression(symbol->span), symbol(symbol) {}

std::shared_ptr<Element> Symbol::evaluate() const {
    throw std::runtime_error("Symbol evaluation is to be implemented");
}

void Symbol::display(std::ostream& stream, size_t depth) const {
    stream << this->symbol->display_verbose(depth);
}

bool Symbol::returns() const { return false; }
bool Symbol::breaks() const { return false; }
bool Symbol::can_evaluate_to(ast::ElementKind) const { return true; }
bool Symbol::can_break_with(ast::ElementKind) const { return false; }
void Symbol::validate_no_free_break() const {}
void Symbol::validate_no_break_with_value() const {}

} // namespace evaluator
