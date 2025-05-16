#pragma once
#pragma once
#include <string>
#include "TokenType.h"  

std::string TokenTypeToString(TokenType type);
std::string LiteralToString(const Literal& lit);
bool isDigit(char c);
bool isAlpha(char c);
bool isAlphaNumeric(char c);