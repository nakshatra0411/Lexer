# Clox

A high-performance, feature-complete procedural **Lox interpreter** written in modern C++20/23, based on the architecture described in *Crafting Interpreters* by Robert Nystrom. 

---

## About Lox

Lox is a clean, dynamically typed scripting language designed for educational interpreters. This implementation focuses on the procedural core of the language, utilizing modern C++ design patterns to bypass traditional interpreter overhead.

### Key Characteristics

- **Dynamically typed variables**: Store numbers, strings, booleans, and callables seamlessly via `std::variant`.
- **Procedural Core & Control Flow**: Full support for blocks, conditionals (`if`/`else`), loops (`while`), and functions with explicit or implicit returns.
- **Lexically Scoped**: Static scoping enforced via an ahead-of-time semantic analysis pass.
- **First-Class Functions & Closures**: Functions capture their defining environment, allowing for full state retention and closures.

---

## Architecture & Technical Highlights

This project moves away from traditional slow, pointer-heavy OOP interpreter designs, leveraging C++ features for speed, safety, and modern idioms:

- **Variant-Based AST Dispatch (`std::visit`)**: Eliminates virtual function table overhead by using `std::visit` paired with the modern `overloaded` pattern for fast, type-safe execution and static analysis.
- **Static Scope Resolution (`Resolver`)**: An ahead-of-time tree-walking semantic analyzer that maps every variable reference to its exact environment "distance" up the scope chain.
- **Cache-Optimized Scopes (`std::flat_map`)**: Local blocks utilize C++23's `std::flat_map` instead of `std::unordered_map`, ensuring contiguous memory layout and superior cache locality for small symbol tables.
- **Environment Chain & Stack Unwinding**: Environments are managed via smart pointer trees (`std::shared_ptr<Environment>`), and function return mechanics are handled via clean exception-based stack unwinding (`ReturnException`).
- **Native Bindings**: Built-in runtime extensions (such as high-resolution benchmarking via `clock()`) injected cleanly into the global scope.

---

## Compilation Pipeline

Source code flows through a rigorous, multi-stage compilation and execution pipeline:

```text
Source Code -> Scanner (Tokens) -> Parser (AST) -> Resolver (Semantic Distance) -> Interpreter (Execution)

```

---

## Components

* **`lang`**: The core driver coordinating source file reading, error reporting, and the execution pipeline.
* **`Scanner`**: Tokenizes raw source files into tokens, handling keywords, literals, and comments.
* **`Parser`**: A recursive-descent parser constructing a structured Abstract Syntax Tree (AST).
* **`Resolver`**: Performs static analysis to compute lexical variable depths and guard against initialization errors.
* **`Interpreter`**: Evaluates AST nodes, manages scopes, handles native calls, and executes block execution chains.
* **`Environment`**: Manages variable bindings and distance-based (`getAt`/`assignAt`) lookups.

---

## Building and Usage

### Requirements

* A modern C++ compiler supporting **C++23** .

### Compilation (Example using MSVC / Clang / GCC)

Compile all source files together specifying the modern standard:

```bash
g++ -std=c++23 main.cpp lang.cpp Scanner.cpp Parser.cpp Resolver.cpp Interpreter.cpp utils.cpp -o Clox

```

### Running a Script

Execute any `.lox` source file directly through the binary:

```bash
./Clox script.lox

```

---

## Future Scope (Planned Enhancements)

* Object-Oriented Programming support (Classes, instances, fields, methods, and inheritance via `this` and `super`).
```

```
