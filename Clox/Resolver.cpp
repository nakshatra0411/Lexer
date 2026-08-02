#include "Resolver.h"
#include "utils.h"

void Resolver::resolve(const Stmt* stmt) {
    if (!stmt) return;

    std::visit(overloaded{
        [&](const BlockStmt& b) {
            beginScope();
            resolve(b.statements);
            endScope();
        },
        [&](const VarStmt& v) {
            declare(v.name);
            if (v.initializer != nullptr) {
                resolve(v.initializer.get());
            }
            define(v.name);
        },
        [&](const FunctionStmt& f) {
            declare(f.name);
            define(f.name);

            beginScope();
            for (const Token& param : f.params) {
                declare(param);
                define(param);
            }
            resolve(f.body);
            endScope();
        },
        [&](const ExpressionStmt& e) {
            resolve(e.expression.get());
        },
        [&](const IfStmt& i) {
            resolve(i.condition.get());
            resolve(i.thenBranch.get());
            if (i.elseBranch != nullptr) {
                resolve(i.elseBranch.get());
            }
        },
        [&](const PrintStmt& p) {
            resolve(p.expression.get());
        },
        [&](const ReturnStmt& r) {
            if (r.value != nullptr) {
                resolve(r.value.get());
            }
        },
        [&](const WhileStmt& w) {
            resolve(w.condition.get());
            resolve(w.body.get());
        }
        }, stmt->as); 
}

void Resolver::resolve(const Expr* expr) {
    if (!expr) return;

    std::visit(overloaded{
        [&](const Variable& v) {
            if (!scopes.empty() && scopes.back().contains(v.name.lexeme) && scopes.back().at(v.name.lexeme) == 0) {
                throw std::runtime_error("Can't read local variable in its own initializer.");
            }
            resolveLocal(expr, v.name);
        },
        [&](const Assign& a) {
            resolve(a.value.get());
            resolveLocal(expr, a.name);
        },
        [&](const Binary& b) {
            resolve(b.left.get());
            resolve(b.right.get());
        },
        [&](const Call& c) {
            resolve(c.callee.get());
            for (const auto& arg : c.arguments) {
                resolve(arg.get());
            }
        },
        [&](const Grouping& g) {
            resolve(g.expression.get());
        },
        [&](const LiteralExpr& l) {
            // Nothing to do for literals
        },
        [&](const Logical& l) {
            resolve(l.left.get());
            resolve(l.right.get());
        },
        [&](const Unary& u) {
            resolve(u.right.get());
        }
        }, expr->as);
}