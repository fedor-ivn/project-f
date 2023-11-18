#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "ast/element.h"
#include "evaluator/error.h"
#include "evaluator/evaluator.h"
#include "evaluator/expression.h"
#include "reader/error.h"
#include "reader/reader.h"

using evaluator::EvaluationError;
using evaluator::Evaluator;
using evaluator::Program;
using reader::Reader;

enum class Mode { SYNTAX, SEMANTIC };

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

        else if (error.cause == ArgumentErrorCause::ExtraArgument) {
            stream << "extra argument: '" << error.argument << "'";
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

            std::cout << "This file is supposed to be syntactically incorrect, "
                         "but it is actucally not.";
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
    } catch (reader::SyntaxError const& e) {
        std::cout << e << std::endl;
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

    std::vector<std::shared_ptr<ast::Element>> empty_program;
    Program program = Program::parse(empty_program);
    try {
        Reader reader(source);
        program = Program::parse(reader.read());
    } catch (EvaluationError const& e) {
        std::cout << std::endl << e << std::endl;
        return false;
    }

    Evaluator evaluator;

    auto output = evaluator.evaluate(std::move(program));

    auto boolean = std::dynamic_pointer_cast<ast::Boolean>(output);

    if (!boolean) {
        return false;
    }

    return boolean->value;
}

std::vector<std::filesystem::path> get_paths(Mode mode) {
    std::vector<std::string_view> subdirectories;

    if (mode == Mode::SEMANTIC) {
        subdirectories.push_back("semantic");
        subdirectories.push_back("functions");
    } else {
        subdirectories.push_back("syntax");
    }

    std::vector<std::filesystem::path> paths;

    for (auto subdirectory : subdirectories) {
        for (auto const& entry : std::filesystem::directory_iterator(
                 "tests/" + std::string(subdirectory)
             )) {
            if (entry.path().extension() == ".lispf") {
                paths.push_back(entry.path());
            }
        }
    }

    return paths;
}

int test_files_semantic(std::vector<std::filesystem::path> paths) {
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

int test_files_syntax(std::vector<std::filesystem::path> paths) {
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
    std::optional<std::string_view> file = std::nullopt;

  public:
    Mode mode;
    bool help = false;

    constexpr static const std::string_view HELP = "--help";
    constexpr static const std::string_view SYNTAX = "--syntax";
    constexpr static const std::string_view SEMANTIC = "--semantic";

    std::optional<std::string_view> get_file() { return this->file; }

    void parse(int argc, char const** argv) {
        if (argc == 1) {
            this->mode = Mode::SEMANTIC;
        }

        if (argc > 1) {
            if (argv[1] == HELP) {
                this->help = true;
                return;
            }
        }

        for (int argument_index = 1; argument_index < argc; argument_index++) {
            std::string_view argument = argv[argument_index];

            if (argument_index == 1) {
                if (argument == SYNTAX) {
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

    if (arguments.help) {
        Arguments::print_help(program_name);
        return 0;
    }

    if (auto path_str = arguments.get_file()) {
        std::filesystem::path path(path_str.value());

        if (!std::filesystem::exists(path)) {
            throw std::runtime_error("provided file does not exist");
        }

        if (!test_syntax_file(path)) {
            std::cout << "Syntax test on " + path.string() + " is failed!\n";
            return 1;
        }
        std::cout << "Syntax test on " + path.string() + " is passed!\n";

        if (arguments.mode == Mode::SYNTAX) {
            return 0;
        }

        if (!test_semantic_file(path)) {
            std::cout << "Semantic test on " + path.string() + " is failed!\n";
            return 1;
        }
        std::cout << "Semantic test on " + path.string() + " is passed!\n";
    } else {
        std::cout << "Syntax tests: \n";
        if (test_files_syntax(get_paths(Mode::SYNTAX))) {
            return 1;
        }

        if (arguments.mode == Mode::SEMANTIC) {
            std::cout << "\nSemantic tests: \n";
            if (test_files_semantic(get_paths(Mode::SEMANTIC))) {
                return 1;
            }
        }
    }

    return 0;
}
