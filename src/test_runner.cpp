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
using reader::SyntaxError;

enum class Mode { SYNTAX, SEMANTIC };

enum class ArgumentErrorCause {
    ExtraArgument,
    WrongFlagPosition,
    UnknownFlag,
};

class ArgumentError : public std::exception {
    ArgumentErrorCause cause;
    std::string argument;

  public:
    ArgumentError(ArgumentErrorCause cause, std::string argument)
        : cause(cause), argument(argument) {}

    friend std::ostream&
    operator<<(std::ostream& stream, ArgumentError const& error) {
        stream << "Error: ";

        if (error.cause == ArgumentErrorCause::ExtraArgument) {
            stream << "extra argument: '" << error.argument << "'";
        }

        else if (error.cause == ArgumentErrorCause::WrongFlagPosition) {
            stream << "wrong flag position: '" << error.argument << "'";
        }

        else if (error.cause == ArgumentErrorCause::UnknownFlag) {
            stream << "unknown flag: '" << error.argument << "'";
        }

        return stream;
    }
};

bool test_fail_file_syntax(std::filesystem::path path) {
    std::ifstream file(path);

    if (!file.is_open()) {
        std::cout << "this file does not exist" << std::endl;
        return false;
    }

    std::string line;

    while (std::getline(file, line)) {
        try {
            Reader reader((std::string_view(line)));
            auto elements = reader.read();

            std::cout << "This file is supposed to be syntactically incorrect, "
                         "but it parsed without errors";
            return false;
        } catch (reader::SyntaxError const&) {
        }
    }

    return true;
}

bool test_correct_file_syntax(std::filesystem::path path) {
    std::ifstream file(path);

    if (!file.is_open()) {
        std::cout << "this file does not exist" << std::endl;
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source(buffer.str());

    Reader reader((std::string_view(source)));
    try {
        auto elements = reader.read();
    } catch (SyntaxError const& e) {
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

    if (!file.is_open()) {
        std::cout << "this file does not exist" << std::endl;
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source(buffer.str());

    std::vector<std::shared_ptr<ast::Element>> empty_program;
    Program program = Program::parse(empty_program);

    Reader reader(source);
    program = Program::parse(reader.read());

    Evaluator evaluator;

    auto output = evaluator.evaluate(std::move(program));

    auto boolean = std::dynamic_pointer_cast<ast::Boolean>(output);

    if (!boolean) {
        return false;
    }

    if (!boolean->value) {
        std::cout << "this expression is evaluated to false\n";
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

        bool passed = false;
        try {
            passed = test_semantic_file(path);
        } catch (SyntaxError const& e) {
            std::cout << e << std::endl;
            code = 1;
        } catch (EvaluationError const& e) {
            std::cout << e << std::endl;
            code = 1;
        }

        if (passed) {
            std::cout << "passed" << std::endl;
        } else {
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
            code = 1;
        }
    }

    return code;
}

class Arguments {
  public:
    std::vector<std::filesystem::path> files;
    Mode mode = Mode::SEMANTIC;
    bool help = false;
    bool explicit_flag = false;

    constexpr static const std::string_view HELP = "--help";
    constexpr static const std::string_view SYNTAX = "--syntax";
    constexpr static const std::string_view SEMANTIC = "--semantic";

    void parse(int argc, char const** argv) {
        if (argc > 1) {
            std::string argument(argv[1]);

            if (argument.starts_with('-')) {
                if (argument == HELP) {
                    this->help = true;
                    return;
                } else if (argument == SYNTAX) {
                    mode = Mode::SYNTAX;
                    explicit_flag = true;
                } else if (argument == SEMANTIC) {
                    mode = Mode::SEMANTIC;
                    explicit_flag = true;
                } else {
                    throw ArgumentError(
                        ArgumentErrorCause::UnknownFlag, argument
                    );
                }
            } else {
                files.push_back(argument);
            }
        }

        for (int argument_index = 2; argument_index < argc; argument_index++) {
            std::string argument = argv[argument_index];

            if (argument == SYNTAX || argument == SEMANTIC) {
                if (explicit_flag) {
                    throw ArgumentError(
                        ArgumentErrorCause::ExtraArgument, argument
                    );
                } else {
                    throw ArgumentError(
                        ArgumentErrorCause::WrongFlagPosition, argument
                    );
                }
            }

            files.push_back(argument);
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
        return 1;
    }

    char const* program_name = argc > 0 ? argv[0] : "test-runner";

    if (arguments.help) {
        Arguments::print_help(program_name);
        return 0;
    }

    int code = 0;

    try {
        if (arguments.files.size()) {
            std::cout << "Tests:\n";
            code = test_files_semantic(arguments.files);
        } else {
            std::cout << "Syntax tests: \n";
            if (test_files_syntax(get_paths(Mode::SYNTAX))) {
                code = 1;
            }

            if (arguments.mode == Mode::SEMANTIC) {
                std::cout << "\nSemantic tests: \n";
                if (test_files_semantic(get_paths(Mode::SEMANTIC))) {
                    code = 1;
                }
            }
        }
    } catch (ArgumentError const& error) {
        std::cerr << error << std::endl;
        return 1;
    }

    return code;
}
