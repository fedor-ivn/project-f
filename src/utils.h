#pragma once

#include <cstddef>
#include <ostream>

#include "ast/element.h"

namespace utils {

class Depth {
    size_t depth;

  public:
    Depth(size_t depth);

    friend std::ostream& operator<<(std::ostream& stream, Depth const& self);
};

std::shared_ptr<ast::Cons> to_cons(std::shared_ptr<ast::List> list);

} // namespace utils
