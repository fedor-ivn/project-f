#include <vector>
#include <filesystem>

class TestRunner {
public:
  std::vector<std::filesystem::path> get_paths();
  void parse_file(std::filesystem::path path);
  void test_all_files();
  bool path_ends_with(std::filesystem::path path, std::string end);
};