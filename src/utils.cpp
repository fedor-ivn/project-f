#include <cstddef>
#include <ostream>

#include "utils.h"

namespace utils {

Depth::Depth(size_t depth) : depth(depth) {}

std::ostream& operator<<(std::ostream& stream, Depth const& self) {
    for (size_t i = 0; i < self.depth; ++i) {
        stream << "  ";
    }
    return stream;
}

std::shared_ptr<ast::Cons> to_cons(std::shared_ptr<ast::List> list) {
    return std::dynamic_pointer_cast<ast::Cons>(list);
}

} // namespace utils
