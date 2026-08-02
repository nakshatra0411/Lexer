#pragma once
#include <string>
#include "Expr.h"
#include "TokenType.h"
#include "lang.h"
#include "Environment.h"
#include "Stmt.h"
#include <unordered_map>


class Interpreter {
public:
    Interpreter(lang* caller);
    void interpret(const std::unique_ptr<Expr>& expression);

private:
    lang* caller;

    // Evaluates an expression and returns a runtime value (Literal)
    Literal evaluate(const std::unique_ptr<Expr>& expr);

    bool isTruthy(const Literal& obj);
    bool isEqual(const Literal& a, const Literal& b);
    void checkNumberOperand(Token op, const Literal& operand);
    void checkNumberOperands(Token op, const Literal& left, const Literal& right);
    std::string stringify(const Literal& object);
public:
    void interpret(const std::vector<std::unique_ptr<Stmt>>& statements);
private:
    std::shared_ptr<Environment> environment{ std::make_shared<Environment>() };
    void execute(const std::unique_ptr<Stmt>& stmt);
    void executeBlock(const std::vector<std::unique_ptr<Stmt>>& statements, std::shared_ptr<Environment> env);

private:
    Literal callCallable(const Callable& callee, const std::vector<Literal>& args, Token paren);

private:
    std::unordered_map<const Expr*, int> locals;
    std::shared_ptr<Environment> globals;
public:
    void resolve(const Expr* expr, int depth) {
        locals[expr] = depth;
    }
    Literal lookupVariable(const Token& name, const Expr* expr);
};