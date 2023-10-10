#include <fstream>
#include <iostream>
#include <filesystem>

#include "test_runner.h"
#include "../reader/reader.h"
#include "../reader/error.h"

using reader::Reader;

bool TestRunner::path_ends_with(std::filesystem::path path, std::string end) {
    if (path.string().length() < end.length()) {
        return false;
    }

    if (path.string().substr(path.string().length() - end.length(), end.length()) == end) {
        return true;
    }
    return false;
}

bool TestRunner::test_fail_file(std::filesystem::path path) {
    
}

void TestRunner::parse_file(std::filesystem::path path) {
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source(buffer.str());

    // if (path_ends_with(path, ".fail.lispf")) {
    //     std::cout << "fail" << std::endl;
    // }
    // else {
    //     std::cout << "norm" << std::endl;
    // }

    Reader reader((std::string_view(source)), 100);
    auto elements = reader.read();
}

void TestRunner::test_all_files() {
    std::vector<std::filesystem::path> paths = get_paths();

    for (auto path : paths) {
        try{
            parse_file(path);
            std::cout << path << ": passed" << std::endl;
        }
        catch (reader::SyntaxError e) {
            std::cout << path << ": " << e << std::endl;
        }
    }
}

std::vector<std::filesystem::path> TestRunner::get_paths() {
    std::vector<std::filesystem::path> paths;

    std::string path = "tests/syntax";
    std::string ext = ".lispf";

    for (const auto & entry : std::filesystem::directory_iterator(path)) {
        if (entry.path().extension() == ext) {
            paths.push_back(entry.path());
        }
    }

    return paths;
}