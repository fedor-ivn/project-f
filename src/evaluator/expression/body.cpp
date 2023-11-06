#include "../../utils.h"
#include "../error.h"
#include "../expression.h"

namespace evaluator {

using ast::Element;
using ast::List;
using ast::Null;
using ast::Position;
using ast::Span;
using utils::Depth;
using utils::to_cons;

Body::Body(std::vector<std::unique_ptr<Expression>> body)
    : body(std::move(body)) {}

Body Body::parse(std::shared_ptr<List> unparsed) {
    std::vector<std::unique_ptr<Expression>> body;

    auto cons = to_cons(unparsed);
    while (cons) {
        body.push_back(Expression::parse(cons->left));
        cons = to_cons(cons->right);
    }
    return Body(std::move(body));
}

std::shared_ptr<Element> Body::evaluate() const {
    std::shared_ptr<Element> last_evaluated =
        std::make_shared<Null>(Null(Span(Position(0, 0), Position(0, 0))));

    for (auto& expression : this->body) {
        last_evaluated = expression->evaluate();
    }
    return last_evaluated;
}

void Body::display(std::ostream& stream, size_t depth) const {
    stream << "Body [\n";

    for (auto const& expression : this->body) {
        stream << Depth(depth + 1);
        expression->display(stream, depth + 1);
        stream << ",\n";
    }

    stream << Depth(depth) << ']';
}

bool Body::can_evaluate_to_function() const {
    return this->body.size() > 0 &&
           this->body.back()->can_evaluate_to_function();
}

bool Body::can_evaluate_to_boolean() const {
    return this->body.size() > 0 &&
           this->body.back()->can_evaluate_to_boolean();
}

void Body::validate_no_free_break() const {
    for (auto const& expression : this->body) {
        expression->validate_no_free_break();
    }
}

void Body::validate_no_break_with_value() const {
    for (auto const& expression : this->body) {
        expression->validate_no_break_with_value();
    }
}

} // namespace evaluator
