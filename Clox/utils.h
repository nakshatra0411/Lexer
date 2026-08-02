#pragma once
#pragma once
#include <string>
#include "TokenType.h"  

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

std::string TokenTypeToString(TokenType type);
std::string LiteralToString(const Literal& lit);
bool isDigit(char c);
bool isAlpha(char c);
bool isAlphaNumeric(char c);