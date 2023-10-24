#include <algorithm>
#include <exception>
#include <memory>

#include "../ast/span.h"
#include "error.h"
#include "parser.h"

namespace reader {

using ast::Span;

Parser::Parser(Scanner scanner) : scanner(scanner) {}

std::unique_ptr<Token>& Parser::peek_token() {
    if (this->peeked == nullptr) {
        this->peeked = this->scanner.next_token();
    }
    return this->peeked;
}

std::unique_ptr<Token> Parser::next_token() {
    if (this->peeked == nullptr) {
        return this->scanner.next_token();
    }
    auto token = std::move(this->peeked);
    this->peeked = nullptr;
    return token;
}

std::unique_ptr<ast::List> Parser::parse_list() {
    auto& token = this->peek_token();
    if (token->is_right_parenthesis()) {
        auto token = this->next_token();
        return std::make_unique<ast::Null>(ast::Null(token->span));
    }

    auto start = token->span.start;
    auto head = this->parse_element();
    auto tail = this->parse_list();
    return std::make_unique<ast::Cons>(
        ast::Cons(std::move(head), std::move(tail), Span(start, tail->span.end))
    );
}

std::unique_ptr<ast::Element> Parser::parse_element() {
    const auto token = this->next_token();

    if (token->is_null()) {
        return std::make_unique<ast::Null>(ast::Null(token->span));
    }
    if (auto boolean = token->to_boolean()) {
        return std::make_unique<ast::Boolean>(
            ast::Boolean(boolean.value(), token->span)
        );
    }
    if (auto integer = token->to_integer()) {
        return std::make_unique<ast::Integer>(
            ast::Integer(integer.value(), token->span)
        );
    }
    if (auto real = token->to_real()) {
        return std::make_unique<ast::Real>(ast::Real(real.value(), token->span)
        );
    }
    if (auto symbol = token->to_symbol()) {
        return std::make_unique<ast::Symbol>(
            ast::Symbol(std::string(symbol.value()), token->span)
        );
    }

    if (token->is_apostrophe()) {
        auto quote =
            std::make_unique<ast::Symbol>(ast::Symbol("quote", token->span));
        auto element = this->parse_element();
        auto end = element->span.end;
        auto tail = std::make_unique<ast::Cons>(ast::Cons(
            std::move(element),
            std::make_shared<ast::Null>(ast::Null(Span(end, end))),
            element->span
        ));
        return std::make_unique<ast::Cons>(ast::Cons(
            std::move(quote),
            std::move(tail),
            Span(token->span.start, tail->span.end)
        ));
    }

    if (token->is_left_parenthesis()) {
        try {
            auto list = this->parse_list();
            list->span.start = token->span.start;
            return list;
        } catch (SyntaxError& error) {
            switch (error.cause) {
            case ErrorCause::UnclosedList:
                throw SyntaxError(
                    ErrorCause::UnclosedList,
                    Span(token->span.start, error.span.end),
                    true
                );
                break;
            default:
                throw error;
            }
        }
    }

    if (token->is_right_parenthesis()) {
        throw SyntaxError(
            ErrorCause::UnexpectedRightParenthesis, token->span, false
        );
    }

    throw SyntaxError(ErrorCause::UnclosedList, token->span, true);
}

std::vector<std::unique_ptr<ast::Element>> Parser::parse() {
    std::vector<std::unique_ptr<ast::Element>> ast;

    while (!this->peek_token()->is_end_of_file()) {
        auto element = this->parse_element();
        ast.push_back(std::move(element));
    }

    return ast;
}

} // namespace reader
