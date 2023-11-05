#include "../expression.h"

namespace evaluator {

using ast::Element;

std::unique_ptr<Expression>
Expression::from_element(std::shared_ptr<Element> element) {
    if (auto cons = element->to_cons()) {
        auto symbol = cons->left->to_symbol();
        auto arguments = cons->right;

        if (symbol == "quote") {
            return Quote::parse(arguments);
        }

        if (symbol == "setq") {
            return Setq::parse(arguments);
        }

        if (symbol == "func") {
            return Func::parse(arguments);
        }

        if (symbol == "lambda") {
            return Lambda::parse(arguments);
        }

        if (symbol == "prog") {
            return Prog::parse(arguments);
        }

        if (symbol == "return") {
            return Return::parse(arguments);
        }

        if (symbol == "while") {
            return While::parse(arguments);
        }

        if (symbol == "break") {
            return Break::parse(arguments);
        }

        if (symbol == "cond") {
            return Cond::parse(arguments);
        }

        return Call::parse(*cons);
    }

    if (auto symbol = std::dynamic_pointer_cast<ast::Symbol>(element)) {
        return std::make_unique<Symbol>(Symbol(std::move(symbol)));
    }

    return std::make_unique<Quote>(Quote(std::move(element)));
}

} // namespace evaluator
