#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "reader/error.h"
#include "reader/reader.h"

using reader::Reader;

bool test_fail_file(std::filesystem::path path) {
    std::ifstream file(path);
    std::string line;

    while (std::getline(file, line)) {
        try {
            Reader reader((std::string_view(line)));
            auto elements = reader.read();
            return false;
        } catch (reader::SyntaxError const&) {
        }
    }

    return true;
}

bool test_correct_file(std::filesystem::path path) {
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source(buffer.str());

    Reader reader((std::string_view(source)));
    try {
        auto elements = reader.read();
    } catch (reader::SyntaxError const&) {
        return false;
    }

    return true;
}

bool test_file(std::filesystem::path path) {
    if (path.string().ends_with(".fail.lispf")) {
        return test_fail_file(path);
    }
    return test_correct_file(path);
}

std::vector<std::filesystem::path> get_paths() {
    std::vector<std::filesystem::path> paths;

    for (auto const& entry :
         std::filesystem::directory_iterator("tests/syntax")) {
        if (entry.path().extension() == ".lispf") {
            paths.push_back(entry.path());
        }
    }

    return paths;
}

int test_all_files() {
    std::vector<std::filesystem::path> paths = get_paths();

    int code = 0;

    for (auto&& path : paths) {
        std::cout << path << ": ";

        bool passed = test_file(path);
        if (passed) {
            std::cout << "passed" << std::endl;
        } else {
            std::cout << "failed" << std::endl;
            code = 1;
        }
    }

    return code;
}

int main() { return test_all_files(); }
