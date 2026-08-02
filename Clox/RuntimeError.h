#include <stdexcept>
#include "Scanner.h"

//The issue is a strict C++ rule regarding exception handling: You cannot throw an incomplete type.

class RuntimeError : public std::runtime_error {
public:
    Token token;
    RuntimeError(Token token, const std::string& message)
        : std::runtime_error(message), token(token) {}
};
