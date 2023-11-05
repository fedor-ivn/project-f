#include "../../utils.h"
#include "../expression.h"

namespace evaluator {

using ast::Element;
using utils::Depth;

Symbol::Symbol(std::shared_ptr<ast::Symbol> symbol)
    : Expression(), symbol(symbol) {}

std::shared_ptr<Element> Symbol::evaluate() const {
    throw std::runtime_error("Symbol evaluation is to be implemented");
}

void Symbol::display(std::ostream& stream, size_t depth) const {
    stream << "Symbol {\n";
    stream << Depth(depth + 1)
           << "symbol = " << this->symbol->display_verbose(depth + 1) << '\n';
    stream << Depth(depth) << '}';
}

bool Symbol::can_evaluate_to_function() const { return true; }
bool Symbol::can_evaluate_to_boolean() const { return true; }

} // namespace evaluator
