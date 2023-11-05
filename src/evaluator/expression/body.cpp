#include "../../utils.h"
#include "../error.h"
#include "../expression.h"

namespace evaluator {

using ast::Element;
using ast::Null;
using ast::Position;
using ast::Span;
using utils::Depth;

Body::Body(std::vector<std::unique_ptr<Expression>> body)
    : body(std::move(body)) {}

Body Body::from_elements(std::vector<std::shared_ptr<Element>> elements) {
    std::vector<std::unique_ptr<Expression>> program;
    for (auto&& element : elements) {
        program.push_back(Expression::from_element(std::move(element)));
    }
    return Body(std::move(program));
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
    return this->body[this->body.size() - 1]->can_evaluate_to_function();
}

bool Body::can_evaluate_to_boolean() const {
    return this->body[this->body.size() - 1]->can_evaluate_to_boolean();
}

} // namespace evaluator
