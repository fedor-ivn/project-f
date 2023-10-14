#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <optional>
#include <ostream>
#include <sstream>

#include "ast/span.h"
#include "evaluator/evaluator.h"
#include "evaluator/program.h"
#include "reader/error.h"
#include "reader/reader.h"
#include "reader/scanner.h"

using ast::Position;
using evaluator::Evaluator;
using evaluator::Program;
using reader::Reader;
using reader::Scanner;
using reader::SyntaxError;

enum class Mode {
    LexicalAnalysis,
    SyntaxAnalysis,
    SemanticAnalysis,
    PrintResult,
    Silent,
    Auto,
};

enum class ArgumentErrorCause {
    UnknownOption,
    ExtraArgument,
};

class ArgumentError : public std::exception {
  public:
    ArgumentErrorCause cause;
    std::string_view argument;

    ArgumentError(ArgumentErrorCause cause, std::string_view argument)
        : cause(cause), argument(argument) {}

    friend std::ostream&
    operator<<(std::ostream& stream, const ArgumentError& error) {
        stream << "Error: ";
        switch (error.cause) {
        case ArgumentErrorCause::UnknownOption:
            stream << "unknown option '" << error.argument << "'";
            break;
        case ArgumentErrorCause::ExtraArgument:
            stream << "extra argument '" << error.argument << "'";
            break;
        }

        return stream;
    }
};

class Arguments {
    constexpr static const std::string_view HELP = "--help";
    constexpr static const std::string_view LEXICAL = "--lexical";
    constexpr static const std::string_view SYNTAX = "--syntax";
    constexpr static const std::string_view SEMANTIC = "--semantic";
    constexpr static const std::string_view PRINT = "--print";
    constexpr static const std::string_view SILENT = "--silent";
    constexpr static const std::string_view AUTO = "--auto";

  public:
    bool help = false;
    Mode mode = Mode::Auto;
    std::optional<std::string_view> file = std::nullopt;

    void parse(int argc, const char** argv) {
        bool is_parsing_options = true;
        for (int at = 1; at < argc; ++at) {
            std::string_view argument(argv[at]);

            if (is_parsing_options && argument.starts_with('-')) {
                if (argument == HELP) {
                    this->help = true;
                    break;
                } else if (argument == "--") {
                    is_parsing_options = false;
                } else if (argument == LEXICAL) {
                    this->mode = Mode::LexicalAnalysis;
                } else if (argument == SYNTAX) {
                    this->mode = Mode::SyntaxAnalysis;
                } else if (argument == SEMANTIC) {
                    this->mode = Mode::SemanticAnalysis;
                } else if (argument == PRINT) {
                    this->mode = Mode::PrintResult;
                } else if (argument == SILENT) {
                    this->mode = Mode::Silent;
                } else if (argument == AUTO) {
                    this->mode = Mode::Auto;
                } else {
                    throw ArgumentError(
                        ArgumentErrorCause::UnknownOption, argument
                    );
                }
                continue;
            }

            if (this->file) {
                throw ArgumentError(
                    ArgumentErrorCause::ExtraArgument, argument
                );
            }
            this->file = argument;
        }
    }

    static void print_help(const char* program_name) {
        std::cerr << "F language interpreter" << std::endl;
        std::cerr << std::endl;
        std::cerr << "Usage: " << program_name << " [...options] [file]"
                  << std::endl;
        std::cerr << std::endl;
        std::cerr << "Options:" << std::endl;
        std::cerr << "\t" << HELP << "\t\tPrint this message" << std::endl;
        std::cerr << "\t" << LEXICAL
                  << "\tTokenize the source code and print the "
                     "tokens. Do not process it further"
                  << std::endl;
        std::cerr << "\t" << SYNTAX
                  << "\tParse the source code and print the AST. Do "
                     "not process it further"
                  << std::endl;
        std::cerr << "\t" << SEMANTIC
                  << "\tParse the AST and print the program. Do "
                     "not process it further"
                  << std::endl;
        std::cerr << "\t" << PRINT
                  << "\t\tEvaluate the code and always print its "
                     "result. This is the default mode for REPL"
                  << std::endl;
        std::cerr << "\t" << SILENT
                  << "\tEvaluate the code but do not print its "
                     "result. This is the default mode for file evaluation"
                  << std::endl;
        std::cerr << "\t" << AUTO
                  << "\t\tKeep the default mode (--print for REPL and "
                     "--silent otherwise)"
                  << std::endl;
    }
};

void print_tokens(Scanner& scanner) {
    while (true) {
        try {
            auto token = scanner.next_token();
            if (token->is_end_of_file()) {
                break;
            }
            std::cout << *token << std::endl;
        } catch (SyntaxError error) {
            std::cout << error << std::endl;
            return;
        }
    }
}

void print_ast(Reader& reader) {
    try {
        auto ast = reader.read();
        for (auto&& element : ast) {
            std::cout << *element << std::endl;
        }
    } catch (SyntaxError error) {
        std::cout << error << std::endl;
        return;
    }
}

void process(Evaluator& evaluator, Reader& reader) {
    auto elements = reader.read();
    auto program = Program::from_elements(std::move(elements));
    auto output = evaluator.evaluate(std::move(program));

    std::cout << *output << std::endl;
}

void repl(Evaluator& evaluator) {
    std::string line;
    size_t nth_line = 0;
    while (true) {
        std::cout << ">>> ";
        if (!std::getline(std::cin, line)) {
            break;
        }

        ++nth_line;
        std::string_view source(line);
        Reader reader((std::string_view(source)), Position(nth_line));
        process(evaluator, reader);
    }
}

int main(int argc, const char** argv) {
    Arguments arguments;
    try {
        arguments.parse(argc, argv);
    } catch (ArgumentError error) {
        std::cerr << error << std::endl;
        return 1;
    }
    if (arguments.help) {
        const char* name = argc > 0 ? argv[0] : "project-f";
        arguments.print_help(name);
        return 0;
    }

    Evaluator evaluator;

    if (arguments.file) {
        if (arguments.mode == Mode::Auto) {
            arguments.mode = Mode::Silent;
        }
        std::ifstream file(std::string(*arguments.file));
        std::stringstream buffer;
        buffer << file.rdbuf();

        std::string source(buffer.str());
        Reader reader((std::string_view(source)));
        process(evaluator, reader);
    } else {
        if (arguments.mode == Mode::Auto) {
            arguments.mode = Mode::PrintResult;
        }
        repl(evaluator);
    }

    return 0;
}
