#include "evaluator.h"
#include "function.h"

namespace evaluator {

using ast::Position;
using ast::Span;

Evaluator::Evaluator() : global(this->garbage_collector.create_scope(nullptr)) {
    Span nowhere(Position(0, 0), Position(0, 0));

    this->global->define(
        ast::Symbol("plus", nowhere), std::make_shared<PlusFunction>()
    );
    this->global->define(
        ast::Symbol("times", nowhere), std::make_shared<TimesFunction>()
    );
    this->global->define(
        ast::Symbol("minus", nowhere), std::make_shared<MinusFunction>()
    );
    this->global->define(
        ast::Symbol("divide", nowhere), std::make_shared<DivideFunction>()
    );

    this->global->define(
        ast::Symbol("head", nowhere), std::make_shared<HeadFunction>()
    );
    this->global->define(
        ast::Symbol("tail", nowhere), std::make_shared<TailFunction>()
    );
    this->global->define(
        ast::Symbol("cons", nowhere), std::make_shared<ConsFunction>()
    );

    this->global->define(
        ast::Symbol("eval", nowhere), std::make_shared<EvalFunction>()
    );

    this->global->define(
        ast::Symbol("equal", nowhere), std::make_shared<EqualFunction>()
    );
    this->global->define(
        ast::Symbol("nonequal", nowhere), std::make_shared<NonequalFunction>()
    );
    this->global->define(
        ast::Symbol("less", nowhere), std::make_shared<LessFunction>()
    );
    this->global->define(
        ast::Symbol("lesseq", nowhere), std::make_shared<LesseqFunction>()
    );
    this->global->define(
        ast::Symbol("greater", nowhere), std::make_shared<GreaterFunction>()
    );
    this->global->define(
        ast::Symbol("greatereq", nowhere), std::make_shared<GreatereqFunction>()
    );

    this->global->define(
        ast::Symbol("isint", nowhere), std::make_shared<IsIntFunction>()
    );
    this->global->define(
        ast::Symbol("isreal", nowhere), std::make_shared<IsRealFunction>()
    );
    this->global->define(
        ast::Symbol("isbool", nowhere), std::make_shared<IsBoolFunction>()
    );
    this->global->define(
        ast::Symbol("isnull", nowhere), std::make_shared<IsNullFunction>()
    );
    this->global->define(
        ast::Symbol("isatom", nowhere), std::make_shared<IsAtomFunction>()
    );
    this->global->define(
        ast::Symbol("islist", nowhere), std::make_shared<IsListFunction>()
    );
    this->global->define(
        ast::Symbol("isfunc", nowhere), std::make_shared<IsFuncFunction>()
    );

    this->global->define(
        ast::Symbol("and", nowhere), std::make_shared<AndFunction>()
    );
    this->global->define(
        ast::Symbol("or", nowhere), std::make_shared<OrFunction>()
    );
    this->global->define(
        ast::Symbol("xor", nowhere), std::make_shared<XorFunction>()
    );
    this->global->define(
        ast::Symbol("not", nowhere), std::make_shared<NotFunction>()
    );
}

ElementGuard Evaluator::evaluate(Program program) {
    return program.evaluate(
        EvaluationContext(&this->garbage_collector, *this->global)
    );
}

} // namespace evaluator
