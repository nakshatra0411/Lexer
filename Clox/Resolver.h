#pragma once
#include <vector>
#include <flat_map> 
#include <string>
#include "Expr.h"
#include "Stmt.h"
#include "Interpreter.h"

class Resolver {
private:
    Interpreter* interpreter;

    std::vector<std::flat_map<std::string, uint8_t>> scopes;

    void beginScope() {
        scopes.push_back(std::flat_map<std::string, uint8_t>{});
    }

    void endScope() {
        scopes.pop_back();
    }

    void declare(const Token& name) {
        if (scopes.empty()) return;
        auto& scope = scopes.back();
        scope[name.lexeme] = 0; // 0 means "declared but not yet ready"
    }

    void define(const Token& name) {
        if (scopes.empty()) return;
        scopes.back()[name.lexeme] = 1; // 1 means "ready for use"
    }

    void resolveLocal(const Expr* expr, const Token& name) {
        // Search backwards through the scopes (innermost to outermost)
        for (int i = static_cast<int>(scopes.size()) - 1; i >= 0; i--) {
            if (scopes[i].contains(name.lexeme)) {
                interpreter->resolve(expr, static_cast<int>(scopes.size()) - 1 - i);
                return;
            }
        }
        // If not found, assume it is global.
    }

public:
    Resolver(Interpreter* interpreter) : interpreter(interpreter) {}

    void resolve(const std::vector<std::unique_ptr<Stmt>>& statements) {
        for (const auto& stmt : statements) {
            resolve(stmt.get());
        }
    }

    void resolve(const Stmt* stmt);
    void resolve(const Expr* expr);
};