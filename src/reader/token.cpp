#include "token.h"

std::ostream &operator<<(std::ostream &stream, const Token &token) {
  if (const LeftParenthesis *t =
          dynamic_cast<const LeftParenthesis *>(&token)) {
    stream << "LeftParenthesis\n";
  }
  else if (const RightParenthesis *t =
          dynamic_cast<const RightParenthesis *>(&token)) {
    stream << "RightParenthesis\n";
  }
  else if (const Identifier *t =
          dynamic_cast<const Identifier *>(&token)) {
    stream << "Identifier(" << t->value << ")";
  }
  else if (const Integer *t =
          dynamic_cast<const Integer *>(&token)) {
    stream << "Integer(" << t->value << ")";
  }
  else if (const Real *t =
          dynamic_cast<const Real *>(&token)) {
    stream << "Real(" << t->value << ")";
    if (static_cast<int64_t>(t->value * 10) % 10 == 0) {
      stream << ".0\n";
    }
    else {
      stream << "\n";
    }
  }
  else if (const Boolean *t =
          dynamic_cast<const Boolean *>(&token)) {
    stream << "Boolean(" << t->value << ")";
  }

  return stream;
}
