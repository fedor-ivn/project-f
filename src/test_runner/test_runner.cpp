#include <filesystem>
#include <fstream>
#include <iostream>

#include "../reader/error.h"
#include "../reader/reader.h"
#include "test_runner.h"

using reader::Reader;

bool TestRunner::path_ends_with(std::filesystem::path path, std::string end) {
    if (path.string().length() < end.length()) {
        return false;
    }

    if (path.string().substr(
            path.string().length() - end.length(), end.length()
        ) == end) {
        return true;
    }
    return false;
}

bool TestRunner::test_fail_file(std::filesystem::path path) {
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source(buffer.str());

    bool passed = true;

    for (auto line : split(source, '\n')) {
        try {
            Reader reader((std::string_view(line)));
            auto elements = reader.read();
            return false;
        } catch (reader::SyntaxError e) {
        }
    }

    return true;
}

std::vector<std::string> TestRunner::split(std::string str, char separator) {
    std::vector<std::string> strings;

    int startIndex = 0;
    int endIndex = 0;
    for (int i = 0; i <= str.size(); i++) {
        if (str[i] == separator || i == str.size()) {
            endIndex = i;
            std::string temp = str.substr(startIndex, endIndex - startIndex);
            startIndex = endIndex + 1;
        }
    }
    return strings;
}

bool TestRunner::test_correct_file(std::filesystem::path path) {
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source(buffer.str());

    bool passed = true;

    Reader reader((std::string_view(source)));
    try {
        auto elements = reader.read();
    } catch (reader::SyntaxError e) {
        passed = false;
    }

    return passed;
}

bool TestRunner::test_file(std::filesystem::path path) {
    if (path_ends_with(path, ".fail.lispf")) {
        return test_fail_file(path);
    }
    return test_correct_file(path);
}

void TestRunner::test_all_files() {
    std::vector<std::filesystem::path> paths = get_paths();

    for (auto path : paths) {
        std::cout << path << ": ";

        bool passed = parse_file(path);
        if (passed) {
            std::cout << "passed" << std::endl;
        } else {
            std::cout << "failed" << std::endl;
        }
    }
}

std::vector<std::filesystem::path> TestRunner::get_paths() {
    std::vector<std::filesystem::path> paths;

    std::string path = "tests/syntax";
    std::string ext = ".lispf";

    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (entry.path().extension() == ext) {
            paths.push_back(entry.path());
        }
    }

    return paths;
}