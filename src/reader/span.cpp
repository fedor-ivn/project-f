#include "span.h"

Position::Position(size_t line, size_t column) : line(line), column(column) {}

void Position::to_next_line() {
    ++this->line;
    this->column = 1;
}

void Position::to_next_column() { ++this->column; }

std::ostream& operator<<(std::ostream& stream, const Position& position) {
    stream << position.line << ':' << position.column;
    return stream;
}

Span::Span(Position start, Position end) : start(start), end(end) {}

std::ostream& operator<<(std::ostream& stream, const Span& span) {
    stream << span.start << ".." << span.end;
    return stream;
}
