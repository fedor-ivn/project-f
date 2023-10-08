#include "reader.h"
#include "parser.h"
#include "scanner.h"

namespace reader {

Reader::Reader(std::string_view source, ast::Position position)
    : source(source), position(position) {}

std::vector<std::unique_ptr<ast::Element>> Reader::read() {
    Scanner scanner(this->source, this->position);
    Parser parser(scanner);
    return parser.parse();
}

} // namespace reader
