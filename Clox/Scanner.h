#pragma once
#include <string>
#include <vector>
#include "utils.h"
#include "TokenType.h"
#include "lang.h"



class Token {
	const TokenType type;
	const std::string lexeme;
	const Literal literal;
	const int line;
public:
	Token(TokenType type1, std::string lexeme1, Literal literal1, int line1);
	std::string toString();
};

class Scanner {
	std::string source;
	std::vector<Token> tokens;
	lang* caller;
	static const std::unordered_map<std::string, TokenType> keywords;
	int start;
	int current;
	int line;
	bool isAtEnd();
	void ScanToken();
	char advance();
	void AddToken(TokenType type);
	void AddToken(TokenType type, Literal lit);
	bool Match(char exp);
	char Peek();
	char PeekNext();
	void str();
	void Number();
	void Identifier();
public:
	Scanner(const std::string& source, lang* c);
	std::vector<Token> ScanTokens();
};