# Clox
# Simple Scripting Language Lexer (Scanner)

This project implements a **lexer/scanner** component for a custom scripting language interpreter. The lexer reads source code input and converts it into a sequence of tokens, which are the basic syntactic units used for further parsing and interpretation.

---

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

Use your preferred C++17 compiler to compile the source files:

```bash
g++ -std=c++17 -o lang main.cpp lang.cpp Scanner.cpp utils.cpp
