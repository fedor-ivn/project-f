#include "reader.h"
#include "parser.h"
#include "scanner.h"
#include "span.h"

Reader::Reader(std::string_view source) : source(source) {}

Reader::Reader(std::string_view source, Position position)
    : source(source), position(position) {}

std::vector<std::unique_ptr<element::Element>> Reader::read() {
    Scanner scanner(this->source, this->position);
    Parser parser(scanner);
    return parser.parse();
}
