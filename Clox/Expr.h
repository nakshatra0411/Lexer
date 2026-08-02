#pragma once


#include <memory>
#include <variant>
#include "TokenType.h" // Assuming this contains your Token and Literal definitions
#include "Scanner.h"

struct Expr; // Forward declaration for recursive pointers

struct Binary {
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;
};

struct Grouping {
    std::unique_ptr<Expr> expression;
};

struct LiteralExpr {
    Literal value;
};

struct Unary {
    Token op;
    std::unique_ptr<Expr> right;
};

struct Variable {
    Token name;
};

struct Assign {
    Token name;
    std::unique_ptr<Expr> value;
};

struct Logical {
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;
};

struct Call {
    std::unique_ptr<Expr> callee;
    Token paren; // The closing parenthesis, for reporting runtime errors
    std::vector<std::unique_ptr<Expr>> arguments;
};


// The wrapper struct holding the variant
struct Expr {
    std::variant<Binary, Grouping, LiteralExpr, Unary, Variable, Assign, Logical, Call> as;

    // Helper constructors for implicit conversion
    template <typename T>
    Expr(T&& expr) : as(std::forward<T>(expr)) {}
};