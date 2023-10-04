#include "parser.h"
#include "element.h"
#include "error.h"
#include "scanner.h"
#include <algorithm>
#include <exception>
#include <memory>

using namespace element;

Parser::Parser(Scanner scanner) : scanner(scanner) {}

std::unique_ptr<token::Token>& Parser::peek_token() {
    if (this->peeked == nullptr) {
        this->peeked = this->scanner.next_token();
    }
    return this->peeked;
}

std::unique_ptr<token::Token> Parser::next_token() {
    if (this->peeked == nullptr) {
        return this->scanner.next_token();
    }
    auto token = std::move(this->peeked);
    this->peeked = nullptr;
    return token;
}

class ReachedEndOfFile {};

std::unique_ptr<Element> Parser::parse_cons() {
    auto& token = this->peek_token();
    if (token->is_right_parenthesis()) {
        auto token = this->next_token();
        return std::make_unique<Null>(Null(token->span));
    }

    auto start = token->span.start;
    auto head = this->parse_element();
    auto tail = this->parse_cons();
    return std::make_unique<Cons>(
        Cons(std::move(head), std::move(tail), Span(start, tail->span.end)));
}

std::unique_ptr<Element> Parser::parse_element() {
    const auto token = this->next_token();

    if (token->is_null()) {
        return std::make_unique<Null>(Null(token->span));
    }
    if (auto boolean = token->to_boolean()) {
        return std::make_unique<Boolean>(Boolean(boolean.value(), token->span));
    }
    if (auto integer = token->to_integer()) {
        return std::make_unique<Integer>(Integer(integer.value(), token->span));
    }
    if (auto real = token->to_real()) {
        return std::make_unique<Real>(Real(real.value(), token->span));
    }
    if (auto identifier = token->to_identifier()) {
        return std::make_unique<Symbol>(
            Symbol(std::string(identifier.value()), token->span));
    }
    
    if (token->is_apostrophe()) {
        auto quote = std::make_unique<Symbol>(Symbol("quote", token->span));
        auto tail =
            std::make_unique<Cons>(Cons(this->parse_element(), token->span));
        return std::make_unique<Cons>(
            Cons(std::move(quote), std::move(tail),
                 Span(token->span.start, tail->span.end)));
    }

    if (token->is_left_parenthesis()) {
        auto cons = this->parse_cons();
        cons->span.start = token->span.start;
        return cons;
    }

    if (token->is_right_parenthesis()) {
        throw SyntaxError(ErrorCause::UnexpectedRightParenthesis, token->span,
                          false);
    }

    throw SyntaxError(ErrorCause::UnclosedList, token->span, true);
}

std::vector<std::unique_ptr<Element>> Parser::parse() {
    std::vector<std::unique_ptr<Element>> ast;

    while (!this->peek_token()->is_end_of_file()) {
        try {
            auto element = this->parse_element();
            ast.push_back(std::move(element));
        } catch (ReachedEndOfFile) {
        }
    }

    return ast;
}
