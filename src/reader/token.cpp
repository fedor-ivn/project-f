#include "token.h"

Token::Token(Span span) : span(span) {}

std::ostream& operator<<(std::ostream& stream, const Token& token) {
    if (const LeftParenthesis* t =
            dynamic_cast<const LeftParenthesis*>(&token)) {
        stream << "LeftParenthesis";
    } else if (const RightParenthesis* t =
                   dynamic_cast<const RightParenthesis*>(&token)) {
        stream << "RightParenthesis";
    } else if (const Identifier* t = dynamic_cast<const Identifier*>(&token)) {
        stream << "Identifier(" << t->value << ")";
    } else if (const Integer* t = dynamic_cast<const Integer*>(&token)) {
        stream << "Integer(" << t->value << ")";
    } else if (const Real* t = dynamic_cast<const Real*>(&token)) {
        stream << "Real(" << t->value << ")";
    } else if (const Boolean* t = dynamic_cast<const Boolean*>(&token)) {
        stream << "Boolean(" << t->value << ")";
    } else if (const Apostrophe* t = dynamic_cast<const Apostrophe*>(&token)) {
        stream << "Apostrophe";
    } else if (const Null* t = dynamic_cast<const Null*>(&token)) {
        stream << "Null";
    }

    stream << " at " << token.span.start << ".." << token.span.end;

    return stream;
}
