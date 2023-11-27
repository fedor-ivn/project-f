#include "../error.h"
#include "../expression.h"
#include "../function.h"

namespace evaluator {

FuncFunction::FuncFunction(
    ast::Span span,
    std::string name,
    Parameters parameters,
    std::shared_ptr<Body> body,
    std::weak_ptr<Scope> scope
)
    : UserDefinedFunction(span, parameters, body, scope), _name(name) {}

std::string_view FuncFunction::name() const { return this->_name; }

void FuncFunction::_display_verbose(std::ostream& stream, size_t) const {
    stream << "FuncFunction(" << this->_name << ", " << this->span << ")";
}

} // namespace evaluator
