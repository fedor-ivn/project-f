#include "../expression.h"

namespace evaluator {

using ast::Cons;
using ast::Element;

std::unique_ptr<Expression> Expression::parse(std::shared_ptr<Element> element
) {
    if (auto cons = std::dynamic_pointer_cast<Cons>(element)) {
        auto symbol = std::dynamic_pointer_cast<ast::Symbol>(cons->left);
        if (symbol) {
            std::string_view name(symbol->value);
            auto arguments = cons->right;

            if (name == "quote") {
                return Quote::parse(arguments);
            }

            if (name == "setq") {
                return Setq::parse(arguments);
            }

            if (name == "func") {
                return Func::parse(arguments);
            }

            if (name == "lambda") {
                return Lambda::parse(arguments);
            }

            if (name == "prog") {
                return Prog::parse(arguments);
            }

            if (name == "return") {
                return Return::parse(arguments);
            }

            if (name == "while") {
                return While::parse(arguments);
            }

            if (name == "break") {
                return Break::parse(arguments);
            }

            if (name == "cond") {
                return Cond::parse(arguments);
            }
        }

        return Call::parse(*cons);
    }

    if (auto symbol = std::dynamic_pointer_cast<ast::Symbol>(element)) {
        return std::make_unique<Symbol>(Symbol(std::move(symbol)));
    }

    return std::make_unique<Quote>(Quote(std::move(element)));
}

} // namespace evaluator
