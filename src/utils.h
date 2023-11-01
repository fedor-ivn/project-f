#include <cstddef>
#include <ostream>

namespace utils {

class Depth {
    size_t depth;

  public:
    Depth(size_t depth);

    friend std::ostream& operator<<(std::ostream& stream, Depth const& self);
};

} // namespace utils
