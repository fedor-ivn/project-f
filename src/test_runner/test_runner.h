#include <vector>
#include <filesystem>

class TestRunner {
public:
  std::vector<std::filesystem::path> get_paths();
  bool parse_file(std::filesystem::path path);
  void test_all_files();
  bool path_ends_with(std::filesystem::path path, std::string end);
  bool test_fail_file(std::filesystem::path path);
  bool test_correct_file(std::filesystem::path path);
  std::vector<std::string> split(std::string str, char separator);
};