# Clox
# Simple Scripting Language Lexer (Scanner)

This project implements a **lexer/scanner** component for a custom scripting language interpreter. The lexer reads source code input and converts it into a sequence of tokens, which are the basic syntactic units used for further parsing and interpretation.

---

## About the Language: Lox

This lexer is designed to scan source code written in a simple interpreted scripting language called **Lox**, introduced in the book *Crafting Interpreters* by Robert Nystrom.

### Key Characteristics of Lox

- **Dynamically typed**: Variables do not require type annotations.
- **Interpreted**: Lox code is executed directly without compiling to machine code.
- **Imperative**: Supports variables, control flow (e.g., `if`, `while`, `for`), functions, and classes.
- **Object-oriented**: Includes basic class and inheritance mechanisms.
- **Syntactically simple**: The grammar and syntax are minimalistic and clean.

### Grammar Type and Implications

- **Regular Grammar (for Lexical Analysis)**  
  The part of the language handled by the lexer (identifiers, keywords, literals, operators, etc.) can be described by **regular grammar**, which means it can be recognized by a finite state machine.  
  This allows the use of efficient scanning techniques without the need for recursive parsing.

- **Context-Free Grammar (for Parsing)**  
  Although the **lexer** works with regular grammar, the full **Lox language grammar** is context-free, which is required for constructs like nested expressions and matching parentheses.  
  The current project only implements the lexer, but it's built with future parser integration in mind.

## Features

- Tokenizes source code into meaningful tokens such as identifiers, keywords, literals (strings, numbers, booleans), and symbols.
- Supports multi-character operators and comments.
- Handles string literals with proper error reporting on unterminated strings.
- Supports keywords such as `if`, `else`, `for`, `while`, `true`, `false`, etc.
- Includes basic error reporting for unexpected characters.
- Uses modern C++17 features like `std::variant` for token literal representation.

---

## Components

- **`lang`**: Main language runner class that reads source files or runs an interactive prompt, and triggers scanning.
- **`Scanner`**: Core lexer class that processes input source code and produces tokens.
- **`Token`**: Represents individual tokens with type, lexeme, literal value, and line number.
- **`TokenType`**: Enum listing all possible token types.
- **`utils`**: Utility functions for token type to string conversions, character classification, and literal to string formatting.

---

**Future plans:**  
This lexer is the foundational step towards building a **complete compiler** for the language, including parsing, semantic analysis, and code generation.

---

## Usage

### Building

Use your preferred C++20 compiler to compile the source files:

```bash
g++ -std=c++20 -o lang main.cpp lang.cpp Scanner.cpp utils.cpp
