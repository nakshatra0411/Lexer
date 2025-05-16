#include <variant>
#include <string>
#include <type_traits>
#include "utils.h"

std::string TokenTypeToString(TokenType type) {
    switch (type) {
    case LEFT_PAREN: return "LEFT_PAREN";
    case RIGHT_PAREN: return "RIGHT_PAREN";
    case LEFT_BRACE: return "LEFT_BRACE";
    case RIGHT_BRACE: return "RIGHT_BRACE";
    case COMMA: return "COMMA";
    case DOT: return "DOT";
    case MINUS: return "MINUS";
    case PLUS: return "PLUS";
    case SEMICOLON: return "SEMICOLON";
    case SLASH: return "SLASH";
    case STAR: return "STAR";
    case BANG: return "BANG";
    case BANG_EQUAL: return "BANG_EQUAL";
    case EQUAL: return "EQUAL";
    case EQUAL_EQUAL: return "EQUAL_EQUAL";
    case GREATER: return "GREATER";
    case GREATER_EQUAL: return "GREATER_EQUAL";
    case LESS: return "LESS";
    case LESS_EQUAL: return "LESS_EQUAL";
    case IDENTIFIER: return "IDENTIFIER";
    case STRING: return "STRING";
    case NUMBER: return "NUMBER";
    case AND: return "AND";
    case CLASS: return "CLASS";
    case ELSE: return "ELSE";
    case FALSE: return "FALSE";
    case FUN: return "FUN";
    case FOR: return "FOR";
    case IF: return "IF";
    case NIL: return "NIL";
    case OR: return "OR";
    case PRINT: return "PRINT";
    case RETURN: return "RETURN";
    case SUPER: return "SUPER";
    case THIS: return "THIS";
    case TRUE: return "TRUE";
    case VAR: return "VAR";
    case WHILE: return "WHILE";
    case EOF_TOKEN: return "EOF_TOKEN";
    default: return "UNKNOWN";
    }
}

std::string LiteralToString(const Literal& lit) {
    return std::visit([](auto&& val) -> std::string {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, std::monostate>) return "nil";
        else if constexpr (std::is_same_v<T, std::string>) return "\"" + val + "\"";
        else if constexpr (std::is_same_v<T, bool>) return val ? "true" : "false";
        else return std::to_string(val);
        }, lit);
}

bool isDigit(char c) {
    return (c >= '0' && c <= '9');
}

bool isAlpha(char c) {
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_');
} 

bool isAlphaNumeric(char c) {
    return (isDigit(c) or isAlpha(c));
}
