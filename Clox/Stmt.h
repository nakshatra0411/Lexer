#pragma once
#include <memory>
#include <variant>
#include <vector>
#include "Expr.h"

struct Stmt;

struct ExpressionStmt {
    std::unique_ptr<Expr> expression;
};

struct PrintStmt {
    std::unique_ptr<Expr> expression;
};

struct VarStmt {
    Token name;
    std::unique_ptr<Expr> initializer;
};

struct BlockStmt {
    std::vector<std::unique_ptr<Stmt>> statements;
};

struct IfStmt {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> thenBranch;
    std::unique_ptr<Stmt> elseBranch; // Can be nullptr
};

struct WhileStmt {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> body;
};

struct FunctionStmt {
    Token name;
    std::vector<Token> params;
    std::vector<std::unique_ptr<Stmt>> body;
};

struct ReturnStmt {
    Token keyword;
    std::unique_ptr<Expr> value; // Can be nullptr if it's just 'return;'
};

struct Stmt {
    std::variant<ExpressionStmt, PrintStmt, VarStmt, BlockStmt, IfStmt, WhileStmt, FunctionStmt, ReturnStmt> as;

    template <typename T>
    Stmt(T&& stmt) : as(std::forward<T>(stmt)) {}
};

