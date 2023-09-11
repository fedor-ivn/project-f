#include <cstdint>
#include <string_view>
#pragma once

class Token {};

class LeftParenthesis : public Token {};
class RightParenthesis : public Token {};

class Identifier : public Token {
public:
  Identifier (std::string_view value) : value(value) {}
private:
  std::string_view value;
};

class Integer : public Token {
public:
  Integer (int64_t value) : value(value) {}
private:
  int64_t value;
};
class Real : public Token {
public:
  Real (double value) : value(value) {}
private:
  double value;
};

class Boolean : public Token {
public:
  Boolean (bool value) : value(value) {}
private:
  bool value;
};

class Apostrophe : public Token {};

class Null : public Token {};

class EndOfFile : public Token {};
