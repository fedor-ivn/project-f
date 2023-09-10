#include <string_view>
#pragma once

class Token {};

class LeftParenthesis : public Token {};
class RightParenthesis : public Token {};

class Identifier : public Token {
  std::string_view value;
};

class Integer : public Token {
  int value;
};
class Real : public Token {
  long double value;
};

class Boolean : public Token {
  bool value;
};

class Apostrophe : public Token {};

class Null : public Token {};