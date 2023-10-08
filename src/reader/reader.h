#pragma once

#include <memory>
#include <string_view>
#include <vector>

#include "../ast/element.h"
#include "../ast/span.h"

class Reader {
  public:
    Reader(std::string_view source, ast::Position position = ast::Position());

    std::vector<std::unique_ptr<ast::Element>> read();

  private:
    std::string_view source;
    ast::Position position;
};
