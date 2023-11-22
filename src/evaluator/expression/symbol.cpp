#include "../expression.h"

namespace evaluator {

Symbol::Symbol(std::shared_ptr<ast::Symbol> symbol)
    : Expression(symbol->span), symbol(symbol) {}

ElementGuard Symbol::evaluate(EvaluationContext context) const {
    return context.garbage_collector->temporary(
        context.scope->lookup(*this->symbol)
    );
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
