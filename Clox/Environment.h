#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include "TokenType.h"
#include "RunTimeError.h"

class Environment {
public:
    std::shared_ptr<Environment> enclosing;
    std::unordered_map<std::string, Literal> values;

    Environment() : enclosing(nullptr) {}
    Environment(std::shared_ptr<Environment> enclosing) : enclosing(std::move(enclosing)) {}

    void define(const std::string& name, Literal value) {
        values[name] = std::move(value);
    }

    Literal get(const Token& name) {
        if (values.contains(name.lexeme)) { // C++20/23 contains()
            return values.at(name.lexeme);
        }
        if (enclosing) return enclosing->get(name);

        throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
    }

    void assign(const Token& name, Literal value) {
        if (values.contains(name.lexeme)) {
            values[name.lexeme] = std::move(value);
            return;
        }
        if (enclosing) {
            enclosing->assign(name, std::move(value));
            return;
        }

        throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
    }

    Literal getAt(int distance, const std::string& name) {
        return ancestor(distance)->values[name];
    }

    void assignAt(int distance, const Token& name, Literal value) {
        ancestor(distance)->values[name.lexeme] = value;
    }
private:
    // walk exactly 'distance' steps up the environment chain
    Environment* ancestor(int distance) {
        Environment* env = this;
        for (int i = 0; i < distance; i++) {
            env = env->enclosing.get();
        }
        return env;
    }
};