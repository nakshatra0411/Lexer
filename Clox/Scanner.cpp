#include <variant>
#include <unordered_map>
#include <string>
#include "utils.h"
#include "TokenType.h"
#include "Scanner.h"
#include "lang.h"

Token::Token(TokenType type1, std::string lexeme1, Literal literal1, int line1) : type(type1), lexeme(lexeme1), literal(literal1), line(line1) {}

std::string Token::toString() {
	return TokenTypeToString(type) + " " + lexeme + " " + LiteralToString(literal);
}

const std::unordered_map<std::string, TokenType> Scanner::keywords = {
	{"and", AND},
	{"class", CLASS},
	{"else", ELSE},
	{"false", FALSE},
	{"for", FOR},
	{"fun", FUN},
	{"if", IF},
	{"nil", NIL},
	{"or", OR},
	{"print", PRINT},
	{"return", RETURN},
	{"super", SUPER},
	{"this", THIS},
	{"true", TRUE},
	{"var", VAR},
	{"while", WHILE}
	};
bool Scanner::isAtEnd() {
		return current >= source.length();
	}
char Scanner::advance() {
		current++;
		return source[current - 1];
	}
void Scanner::AddToken(TokenType type) {
		AddToken(type, std::monostate{});
	}
void Scanner::AddToken(TokenType type, Literal lit) {
		std::string text = source.substr(start, current-start);
		tokens.emplace_back(type, text, lit, line);
	}
bool Scanner::Match(char exp) {
		if(isAtEnd()) return false;
		if(source[current] != exp) return false;
		current++;
		return true;
 	}
char Scanner::Peek() {
		if (isAtEnd()) return '\0';
		return source[current];
	}
char Scanner::PeekNext() {
		if (current + 1 >= source.length()) return '\0';
		return source[current + 1];
	}
void Scanner::str() {
		while (Peek() != '"' && !isAtEnd()) {
			if (Peek() == '\n') line++;
			advance();
		}
		if (isAtEnd()) {
			caller->Report(line, "Unterminated string");
			return;
		}
		advance();
		std::string value = source.substr(start + 1, current - 2 - start);
		AddToken(STRING, value);
	}
void Scanner::Number() {
		while (isDigit(Peek())) advance();

		// Fraction
		if (Peek() == '.' && isDigit(PeekNext())) {
			// Consume "."
			advance();
			while (isDigit(Peek())) advance();
		}
		AddToken(NUMBER,
			std::stod(source.substr(start, current-start)));

	}
void Scanner::Identifier() {
		while (isAlphaNumeric(Peek())) advance();
		std::string text = source.substr(start, current-start);
		auto it = keywords.find(text);
		TokenType type = (it == keywords.end()) ? IDENTIFIER: it->second;
		AddToken(type);
	}
void Scanner::ScanToken() {
		char c = advance();
		switch (c) {
		// lexemes
		case '(': AddToken(LEFT_PAREN); break; 
		case ')': AddToken(RIGHT_PAREN); break; 
		case '{': AddToken(LEFT_BRACE); break; 
		case '}': AddToken(RIGHT_BRACE); break;
		case ',': AddToken(COMMA); break; 
		case '.': AddToken(DOT); break; 
		case '-': AddToken(MINUS); break; 
		case '+': AddToken(PLUS); break; 
		case ';': AddToken(SEMICOLON); break; 
		case '*': AddToken(STAR); break; 
		case '!':
			AddToken(Match('=') ? BANG_EQUAL : BANG);
			break;
		case '=':
			AddToken(Match('=') ? EQUAL_EQUAL : EQUAL);
			break;
		case '<':
			AddToken(Match('=') ? LESS_EQUAL : LESS);
			break;
		case '>':
			AddToken(Match('=') ? GREATER_EQUAL : GREATER);
			break;
		case '/':
			if (Match('/')) {
				// A comment goes until the end of the line.
				while (Peek() != '\n' && !isAtEnd()) advance();
			}
			else {
				AddToken(SLASH);
			}
			break;
		case ' ':
		case '\r':
		case '\t':
			// Ignore whitespace.
			break;
		case '\n':
			line++;
			break;

		// literals
		case '"': str(); break;
		default: 
			if (isDigit(c)) {
				Number();
			}
			else if (isAlpha(c)) {
				Identifier();
			}
			else {
				caller->Report(line, "Unexpected character.\n");
			}
				break; 
		}
	}

	
Scanner::Scanner(const std::string& sour, lang* c) {
	source = sour;
	start = 0; current = 0; line = 1;
	caller = c;
}

std::vector<Token> Scanner::ScanTokens() {
	while (!isAtEnd()) {
		start = current;
		ScanToken();
	}

	tokens.emplace_back(EOF_TOKEN, "", std::monostate{}, line);
	return tokens;
}
