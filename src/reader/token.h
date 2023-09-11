#include <cstdint>
#include <string_view>
#pragma once

class Token {};

class LeftParenthesis : public Token {};
class RightParenthesis : public Token {};

class Identifier : public Token {
public:
  std::string_view value;

  Identifier (std::string_view value) : value(value) {}
};

class Integer : public Token {
public:
  int64_t value;

  Integer (int64_t value) : value(value) {}
};

class Real : public Token {
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
