#include <algorithm>

#include "../../utils.h"
#include "../error.h"
#include "../expression.h"

namespace evaluator {

using ast::List;
using utils::Depth;
using utils::to_cons;

Parameters::Parameters(std::vector<std::shared_ptr<ast::Symbol>> parameters)
    : parameters(std::move(parameters)) {}

Parameters Parameters::parse(std::shared_ptr<List> parameter_list) {
    std::vector<std::shared_ptr<ast::Symbol>> parameters;

    auto cons = to_cons(parameter_list);
    while (cons) {
        auto parameter = std::dynamic_pointer_cast<ast::Symbol>(cons->left);
        if (!parameter) {
            throw EvaluationError(
                "a parameter must be a symbol", cons->left->span
            );
        }

        auto duplicated = std::find_if(
            parameters.begin(),
            parameters.end(),
            [&parameter](auto& other) {
                return other->value == parameter->value;
            }
        );
        if (duplicated != parameters.end()) {
            std::string message =
                "parameter `" + parameter->value + "` is duplicated";
            throw EvaluationError(message, cons->left->span);
        }

        parameters.push_back(parameter);
        cons = to_cons(cons->right);
    }

    return Parameters(std::move(parameters));
}

void Parameters::display(std::ostream& stream, size_t depth) const {
    stream << "Parameters [\n";
    for (auto const& parameter : this->parameters) {
        stream << Depth(depth + 1) << parameter->display_verbose(depth + 1)
               << ",\n";
    }
    stream << Depth(depth) << ']';
}

} // namespace evaluator
