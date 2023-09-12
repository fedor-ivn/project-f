#pragma once

#include <cstdint>
#include <ostream>
#include <string_view>

class Token {
public:
  virtual ~Token() = default;

  friend std::ostream &operator<<(std::ostream &stream, const Token &token);
};

class LeftParenthesis : public Token {};
class RightParenthesis : public Token {};

class Identifier : public Token {
public:
  std::string_view value;

  Identifier (std::string_view value) : value(value) {}
};

class Numeral : public Token {};

class Integer : public Numeral {
public:
  int64_t value;

  Integer (int64_t value) : value(value) {}
};

class Real : public Numeral {
public:
  double value;

  Real (double value) : value(value) {}
};

class Boolean : public Token {
public:
  bool value;

  Boolean (bool value) : value(value) {}
};

class Apostrophe : public Token {};

class Null : public Token {};

class EndOfFile : public Token {};
