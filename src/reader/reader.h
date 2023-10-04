#pragma once

#include <memory>
#include <string_view>
#include <vector>

#include "element.h"

class Reader {
  public:
    Reader(std::string_view source, Position position = Position());
    
    std::vector<std::unique_ptr<element::Element>> read();

  private:
    std::string_view source;
    Position position = Position();
};
