#pragma once
#include <variant>
#include <string>
#include <vector>
#include <memory>
#include <functional>

// Forward declarations for external components
class Interpreter;
class Environment;
struct FunctionStmt;

// 1. Forward declare the callable structs (incomplete types)
struct NativeFunction;
struct LoxFunction;

// 2. Define Callable using shared_ptrs to those incomplete types
using Callable = std::variant<
	std::shared_ptr<NativeFunction>,
	std::shared_ptr<LoxFunction>
>;

// 3. Define Literal. It is now perfectly valid!
using Literal = std::variant<
	std::monostate,
	double,
	std::string,
	bool,
	Callable   // Injected seamlessly
>;

// 4. Now that Literal is fully defined, we can provide the full struct definitions
struct NativeFunction {
	size_t arity_count;
	std::function<Literal(Interpreter*, const std::vector<Literal>&)> body;
};

struct LoxFunction {
	const FunctionStmt* declaration; // <-- Add 'const' here!
	std::shared_ptr<Environment> closure;
};
enum TokenType {
	// Single-character tokens.
	LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
	COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,
	// One or two character tokens.
	BANG, BANG_EQUAL,
	EQUAL, EQUAL_EQUAL,
	GREATER, GREATER_EQUAL,
	LESS, LESS_EQUAL,
	// Literals.
	IDENTIFIER, STRING, NUMBER,
	// Keywords.
	AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
	PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,
	EOF_TOKEN
};