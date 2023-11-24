#include "../error.h"
#include "../expression.h"
#include "../function.h"
#include <string_view>

namespace evaluator {

std::string_view FuncFunction::name() const { return this->_name; }

void FuncFunction::_display_verbose(std::ostream& stream, size_t) const {
    stream << "FuncFunction(" << this->_name << ", " << this->span << ")";
}

} // namespace evaluator
