#pragma once

#include <vector>
#include <memory>
#include <stdexcept>
#include <initializer_list>
#include "TokenType.h"
#include "Expr.h"
#include "lang.h" // To access your error reporter
#include "Stmt.h"

class ParseError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class Parser {
public:
    Parser(const std::vector<Token>& tokens, lang* caller);
    std::vector<std::unique_ptr<Stmt>> parse();

private:
    std::vector<Token> tokens;
    int current = 0;
    lang* caller;

    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> equality();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> unary();
    std::unique_ptr<Expr> primary();
    std::unique_ptr<Expr> logicalOr();
    std::unique_ptr<Expr> logicalAnd();

    bool match(std::initializer_list<TokenType> types);
    bool check(TokenType type);
    Token advance();
    bool isAtEnd();
    Token peek();
    Token previous();
    Token consume(TokenType type, const std::string& message);
    ParseError error(Token token, const std::string& message);
    void synchronize();
    std::unique_ptr<Stmt> declaration();
    std::unique_ptr<Stmt> varDeclaration();
    std::unique_ptr<Stmt> statement();
    std::unique_ptr<Stmt> printStatement();
    std::unique_ptr<Stmt> expressionStatement();
    std::vector<std::unique_ptr<Stmt>> block();
    std::unique_ptr<Expr> assignment();
    std::unique_ptr<Stmt> ifStatement();
    std::unique_ptr<Stmt> whileStatement();
    std::unique_ptr<Stmt> forStatement();

    std::unique_ptr<Expr> call();
    std::unique_ptr<Expr> finishCall(std::unique_ptr<Expr> callee);
    std::unique_ptr<Stmt> function(const std::string& kind);
    std::unique_ptr<Stmt> returnStatement();
    
};