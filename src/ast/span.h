#pragma once

#include <compare>
#include <cstddef>
#include <ostream>
#include <string>

namespace ast {

class Position {
  public:
    size_t line;
    size_t column;

    Position(size_t line = 1, size_t column = 1);
    auto operator<=>(Position const&) const = default;

    void to_next_line();
    void to_next_column();

    friend std::ostream&
    operator<<(std::ostream& stream, Position const& position);
};

class Span {
  public:
    Position start;
    Position end;

    Span(Position start, Position end);

    friend std::ostream& operator<<(std::ostream& stream, Span const& span);
};

} // namespace ast
