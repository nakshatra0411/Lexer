#include "Interpreter.h"
#include "utils.h"
#include <iostream>
#include <print>  
#include <ranges> 
#include <chrono>
#include <exception>

struct ReturnException : public std::exception {
    Literal value;

    explicit ReturnException(Literal value) : value(std::move(value)) {}
};

Interpreter::Interpreter(lang* caller) : caller(caller) {
    globals = std::make_shared<Environment>();
    environment = globals;

    auto clock_func = std::make_shared<NativeFunction>();
    clock_func->arity_count = 0; // clock() takes no arguments

    clock_func->body = [](Interpreter* interpreter, const std::vector<Literal>& args) -> Literal {
        auto now = std::chrono::system_clock::now().time_since_epoch();
        return std::chrono::duration_cast<std::chrono::duration<double>>(now).count();
        };

    globals->define("clock", Callable{ clock_func });

}

void Interpreter::interpret(const std::vector<std::unique_ptr<Stmt>>& statements) {
    try {
        for (const auto& statement : statements) {
            execute(statement);
        }
    }
    catch (const RuntimeError& error) {
        caller->Report(error.token.line, error.what());
    }
}

void Interpreter::execute(const std::unique_ptr<Stmt>& stmt) {
    std::visit(overloaded{
        [&](const ExpressionStmt& e) {
            evaluate(e.expression);
        },
        [&](const PrintStmt& p) {
            Literal value = evaluate(p.expression);
            // C++23 std::println replaces std::cout!
            std::println("{}", stringify(value));
        },
        [&](const VarStmt& v) {
            Literal value = std::monostate{};
            if (v.initializer) {
                value = evaluate(v.initializer);
            }
            environment->define(v.name.lexeme, std::move(value));
        },
        [&](const BlockStmt& b) {
            executeBlock(b.statements, std::make_shared<Environment>(environment));
        },
        [&](const IfStmt& i) {
            if (isTruthy(evaluate(i.condition))) {
                execute(i.thenBranch);
            }
             else if (i.elseBranch) {
              execute(i.elseBranch);
            }
        },
        [&](const WhileStmt& w) {
            while (isTruthy(evaluate(w.condition))) {
                execute(w.body);
            }
        },
        [&](const FunctionStmt& f) {
            auto lox_func = std::make_shared<LoxFunction>();
            lox_func->declaration = &f;
            lox_func->closure = environment; // Capture the current environment!

            environment->define(f.name.lexeme, Callable{lox_func});
        },
        [&](const ReturnStmt& r) {
            Literal value = std::monostate{};
            if (r.value != nullptr) { // bare return
                value = evaluate(r.value);
            }
            throw ReturnException(value); // Unwind the stack!
        }
        }, stmt->as);
}

void Interpreter::executeBlock(const std::vector<std::unique_ptr<Stmt>>& statements, std::shared_ptr<Environment> env) {
    auto previous = environment;
    try {
        environment = std::move(env);
        // Using C++ ranges to iterate
        for (const auto& statement : statements | std::views::all) {
            execute(statement);
        }
        environment = previous;
    }
    catch (...) {
        environment = previous;
        throw;
    }
}

Literal Interpreter::evaluate(const std::unique_ptr<Expr>& expr) {
    return std::visit(overloaded{
        [&](const LiteralExpr& l) -> Literal {
            return l.value;
        },
        [&](const Grouping& g) -> Literal {
            return evaluate(g.expression);
        },
        [&](const Unary& u) -> Literal {
            Literal right = evaluate(u.right);

            switch (u.op.type) {
                case BANG:
                    return !isTruthy(right);
                case MINUS:
                    checkNumberOperand(u.op, right);
                    return -std::get<double>(right);
                default:
                    return std::monostate{};
            }
        },
        [&](const Binary& b) -> Literal {
            Literal left = evaluate(b.left);
            Literal right = evaluate(b.right);

            switch (b.op.type) {
                case MINUS:
                    checkNumberOperands(b.op, left, right);
                    return std::get<double>(left) - std::get<double>(right);
                case SLASH:
                    checkNumberOperands(b.op, left, right);
                    return std::get<double>(left) / std::get<double>(right);
                case STAR:
                    checkNumberOperands(b.op, left, right);
                    return std::get<double>(left) * std::get<double>(right);
                case PLUS:
                    if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                        return std::get<double>(left) + std::get<double>(right);
                    }
                    if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right)) {
                        return std::get<std::string>(left) + std::get<std::string>(right);
                    }
                    throw RuntimeError(b.op, "Operands must be two numbers or two strings.");
                case GREATER:
                    checkNumberOperands(b.op, left, right);
                    return std::get<double>(left) > std::get<double>(right);
                case GREATER_EQUAL:
                    checkNumberOperands(b.op, left, right);
                    return std::get<double>(left) >= std::get<double>(right);
                case LESS:
                    checkNumberOperands(b.op, left, right);
                    return std::get<double>(left) < std::get<double>(right);
                case LESS_EQUAL:
                    checkNumberOperands(b.op, left, right);
                    return std::get<double>(left) <= std::get<double>(right);
                case BANG_EQUAL:
                    return !isEqual(left, right);
                case EQUAL_EQUAL:
                    return isEqual(left, right);
                default:
                    return std::monostate{};
            }
        },
        [&](const Variable& v) -> Literal {
            return lookupVariable(v.name, expr.get());
        },
        [&](const Assign& a) -> Literal {
            Literal value = evaluate(a.value);

            auto it = locals.find(expr.get());
            if (it != locals.end()) {
                int distance = it->second;
                environment->assignAt(distance, a.name, value);
            } else {
                globals->assign(a.name, value);
            }
            return value;
        },
        [&](const Logical& l) -> Literal {
            Literal left = evaluate(l.left);
            if (l.op.type == OR) {
                if (isTruthy(left)) return left;
            }
             else { // AND
              if (!isTruthy(left)) return left;
            }
            return evaluate(l.right);
        },
        [&](const Call& c) -> Literal {
            // 1. Evaluate the left side of the '(' 
            Literal callee = evaluate(c.callee);

            // 2. Evaluate all the arguments
            std::vector<Literal> arguments;
            for (const auto& arg : c.arguments) {
                arguments.push_back(evaluate(arg));
            }

            // 3. Ensure the callee is actually a function (Callable)
            if (!std::holds_alternative<Callable>(callee)) {
                throw RuntimeError(c.paren, "Can only call functions and classes.");
            }

            // 4. Extract the Callable and invoke it!
            return callCallable(std::get<Callable>(callee), arguments, c.paren);
        }
        }, expr->as);
}

Literal Interpreter::callCallable(const Callable& callee, const std::vector<Literal>& args, Token paren) {
    return std::visit(overloaded{
        [&](const std::shared_ptr<NativeFunction>& native) -> Literal {
            if (args.size() != native->arity_count) {
                throw RuntimeError(paren, "Expected " + std::to_string(native->arity_count) +
                                          " arguments but got " + std::to_string(args.size()) + ".");
            }
            return native->body(this, args);
        },
        [&](const std::shared_ptr<LoxFunction>& func) -> Literal {
            if (args.size() != func->declaration->params.size()) {
                 throw RuntimeError(paren, "Expected " + std::to_string(func->declaration->params.size()) +
                                           " arguments but got " + std::to_string(args.size()) + ".");
            }

            // 1. Create a new environment bound to the function's closure
            auto env = std::make_shared<Environment>(func->closure);

            // 2. Bind the arguments to the parameter names
            for (size_t i = 0; i < func->declaration->params.size(); ++i) {
                env->define(func->declaration->params[i].lexeme, args[i]);
            }

            // 3. Execute the function body, catching the Return exception if thrown
            try {
                executeBlock(func->declaration->body, env);
            } catch (const ReturnException& returnValue) {
                return returnValue.value;
            }

             // 4. Implicit return nil if the function finishes without a return statement
             return std::monostate{};
            }
        }, callee);
}

// --- Semantic Helpers ---
bool Interpreter::isTruthy(const Literal& obj) {
    if (std::holds_alternative<std::monostate>(obj)) return false;
    if (std::holds_alternative<bool>(obj)) return std::get<bool>(obj);
    return true; // Everything else is true in Lox
}

bool Interpreter::isEqual(const Literal& a, const Literal& b) {
    return a == b;
}

void Interpreter::checkNumberOperand(Token op, const Literal& operand) {
    if (std::holds_alternative<double>(operand)) return;
    throw RuntimeError(op, "Operand must be a number.");
}

void Interpreter::checkNumberOperands(Token op, const Literal& left, const Literal& right) {
    if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) return;
    throw RuntimeError(op, "Operands must be numbers.");
}

std::string Interpreter::stringify(const Literal& object) {
    if (std::holds_alternative<std::monostate>(object)) return "nil";
    if (std::holds_alternative<double>(object)) {
        std::string text = std::to_string(std::get<double>(object));
        // Strip trailing zeros to match Lox's required double formatting
        text.erase(text.find_last_not_of('0') + 1, std::string::npos);
        if (text.back() == '.') text.pop_back();
        return text;
    }
    if (std::holds_alternative<bool>(object)) {
        return std::get<bool>(object) ? "true" : "false";
    }
    if (std::holds_alternative<std::string>(object)) {
        return std::get<std::string>(object);
    }
    if (std::holds_alternative<Callable>(object)) {
        const Callable& callable = std::get<Callable>(object);
        if (std::holds_alternative<std::shared_ptr<NativeFunction>>(callable)) {
            return "<native fn>";
        }
        if (std::holds_alternative<std::shared_ptr<LoxFunction>>(callable)) {
            return "<fn>"; // We will improve this when we implement user functions!
        }
    }
    return "unknown";
}

Literal Interpreter::lookupVariable(const Token& name, const Expr* expr) {
    auto it = locals.find(expr);
    if (it != locals.end()) {
        int distance = it->second;
        return environment->getAt(distance, name.lexeme);
    }
    else {
        return globals->get(name);
    }
}