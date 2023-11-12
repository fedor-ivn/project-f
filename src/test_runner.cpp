#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "evaluator/evaluator.h"
#include "evaluator/expression.h"
#include "reader/error.h"
#include "reader/parser.h"
#include "reader/reader.h"
#include "reader/scanner.h"

using evaluator::Evaluator;
using evaluator::Program;
using reader::Parser;
using reader::Reader;
using reader::Scanner;

enum class Mode { HELP, SYNTAX, SEMANTIC };

enum class ArgumentErrorCause { UnknownArgument, ExtraArgument };

class ArgumentError : public std::exception {
    ArgumentErrorCause cause;
    std::string_view argument;

  public:
    ArgumentError(ArgumentErrorCause cause, std::string_view argument)
        : cause(cause), argument(argument) {}

    friend std::ostream&
    operator<<(std::ostream& stream, ArgumentError const& error) {
        stream << "Error: ";

        if (error.cause == ArgumentErrorCause::UnknownArgument) {
            stream << "unknown argument: '" << error.argument << "'";
        }

        return stream;
    }
};

bool test_fail_file_syntax(std::filesystem::path path) {
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

bool test_correct_file_syntax(std::filesystem::path path) {
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

bool test_syntax_file(std::filesystem::path path) {
    if (path.string().ends_with(".fail.lispf")) {
        return test_fail_file_syntax(path);
    }
    return test_correct_file_syntax(path);
}

bool test_semantic_file(std::filesystem::path path) {
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source(buffer.str());

    Scanner scanner(source, ast::Position());
    Parser parser(scanner);

    auto ast = parser.parse();
    auto program = Program::parse(ast);

    Evaluator evaluator;

    auto output = evaluator.evaluate(std::move(program));

    auto boolean = std::dynamic_pointer_cast<ast::Boolean>(output);

    if (!boolean) {
        throw std::runtime_error("test should have a boolean result");
    }

    return boolean->value;
}

std::vector<std::filesystem::path> get_paths(Mode mode) {
    if (mode == Mode::HELP) {
        throw std::runtime_error("get_paths got wrong mode - help");
    }

    std::string postfix = mode == Mode::SEMANTIC ? "semantic" : "syntax";

    std::vector<std::filesystem::path> paths;

    for (auto const& entry :
         std::filesystem::directory_iterator("tests/" + postfix)) {
        if (entry.path().extension() == ".lispf") {
            paths.push_back(entry.path());
        }
    }

    return paths;
}

int test_all_files_semantic() {
    std::vector<std::filesystem::path> paths = get_paths(Mode::SEMANTIC);

    int code = 0;

    for (auto&& path : paths) {
        std::cout << path << ": ";

        bool passed = test_semantic_file(path);
        if (passed) {
            std::cout << "passed" << std::endl;
        } else {
            std::cout << "failed" << std::endl;
            code = 1;
        }
    }

    return code;
}

int test_all_files_syntax() {
    std::vector<std::filesystem::path> paths = get_paths(Mode::SYNTAX);

    int code = 0;

    for (auto&& path : paths) {
        std::cout << path << ": ";

        bool passed = test_syntax_file(path);
        if (passed) {
            std::cout << "passed" << std::endl;
        } else {
            std::cout << "failed" << std::endl;
            code = 1;
        }
    }

    return code;
}

class Arguments {
    constexpr static const std::string_view HELP = "--help";
    constexpr static const std::string_view SYNTAX = "--syntax";
    constexpr static const std::string_view SEMANTIC = "--semantic";

    Mode mode;
    std::optional<std::string_view> file = std::nullopt;

  public:
    std::optional<std::string_view> get_file() { return this->file; }

    void parse(int argc, char const** argv) {
        for (int argument_index = 1; argument_index < argc; argument_index++) {
            std::string_view argument = argv[argument_index];

            if (argument_index == 1) {
                if (argument == HELP) {
                    this->mode = Mode::HELP;
                } else if (argument == SYNTAX) {
                    this->mode = Mode::SYNTAX;
                } else if (argument == SEMANTIC) {
                    this->mode = Mode::SEMANTIC;
                } else {
                    throw ArgumentError(
                        ArgumentErrorCause::UnknownArgument, argument
                    );
                }
            } else {
                if (this->file) {
                    throw ArgumentError(
                        ArgumentErrorCause::ExtraArgument, argument
                    );
                }

                this->file = argument;
            }
        }
    }

    Mode get_mode() { return this->mode; }

    static void print_help(char const* program_name) {
        std::cerr << "F language test runner\n\n";
        std::cerr << "Usage: " << program_name << " [...options]\n\n";
        std::cerr << "Options:\n";
        std::cerr << "\t" << HELP << "\t\tPrint this message\n";
        std::cerr << "\t" << SYNTAX << "\tRun syntax tests\n";
        std::cerr << "\t" << SEMANTIC << "\tRun semantic tests\n";
    }
};

int main(int argc, char const** argv) {
    Arguments arguments;

    try {
        arguments.parse(argc, argv);
    } catch (ArgumentError const& error) {
        std::cerr << error << std::endl;
    }

    char const* program_name = argc > 0 ? argv[0] : "test-runner";

    if (arguments.get_mode() == Mode::HELP) {
        Arguments::print_help(program_name);
    }

    if (auto path_str = arguments.get_file()) {
        std::filesystem::path path(path_str.value());

        if (!std::filesystem::exists(path)) {
            throw std::runtime_error("provided file does not exist");
        }

        bool code;

        if (arguments.get_mode() == Mode::SYNTAX) {
            code = test_syntax_file(path);
        } else {
            code = test_semantic_file(path);
        }

        if (code) {
            std::cout << "Passed!" << std::endl;
        } else {
            std::cout << "Failed!" << std::endl;
        }
    } else {
        if (arguments.get_mode() == Mode::SYNTAX) {
            test_all_files_syntax();
        } else if (arguments.get_mode() == Mode::SEMANTIC) {
            test_all_files_semantic();
        }
    }

    return 0;
}
