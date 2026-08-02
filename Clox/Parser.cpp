#include "Parser.h"

Parser::Parser(const std::vector<Token>& tokens, lang* caller)
    : tokens(tokens), caller(caller) {
}

std::vector<std::unique_ptr<Stmt>> Parser::parse() {
    std::vector<std::unique_ptr<Stmt>> statements;
    while (!isAtEnd()) {
        statements.push_back(declaration());
    }
    return statements;
}

// --- Statements & Declarations ---

std::unique_ptr<Stmt> Parser::declaration() {
    try {
        if (match({ FUN })) return function("function"); 
        if (match({ VAR })) return varDeclaration();
        return statement();
    }
    catch (ParseError error) {
        synchronize();
        return nullptr;
    }
}

std::unique_ptr<Stmt> Parser::varDeclaration() {
    Token name = consume(IDENTIFIER, "Expect variable name.");
    std::unique_ptr<Expr> initializer = nullptr;

    if (match({ EQUAL })) {
        initializer = expression();
    }

    consume(SEMICOLON, "Expect ';' after variable declaration.");
    return std::make_unique<Stmt>(VarStmt{ name, std::move(initializer) });
}

std::unique_ptr<Stmt> Parser::statement() {
    if (match({ FOR })) return forStatement();
    if (match({ IF })) return ifStatement();
    if (match({ PRINT })) return printStatement();
    if (match({ RETURN })) return returnStatement(); 
    if (match({ WHILE })) return whileStatement();
    if (match({ LEFT_BRACE })) return std::make_unique<Stmt>(BlockStmt{ block() });

    return expressionStatement();
}

std::unique_ptr<Stmt> Parser::printStatement() {
    auto value = expression();
    consume(SEMICOLON, "Expect ';' after value.");
    return std::make_unique<Stmt>(PrintStmt{ std::move(value) });
}

std::unique_ptr<Stmt> Parser::expressionStatement() {
    auto expr = expression();
    consume(SEMICOLON, "Expect ';' after expression.");
    return std::make_unique<Stmt>(ExpressionStmt{ std::move(expr) });
}

std::vector<std::unique_ptr<Stmt>> Parser::block() {
    std::vector<std::unique_ptr<Stmt>> statements;

    while (!check(RIGHT_BRACE) && !isAtEnd()) {
        statements.push_back(declaration());
    }

    consume(RIGHT_BRACE, "Expect '}' after block.");
    return statements;
}

// --- Expressions ---

std::unique_ptr<Expr> Parser::expression() {
    return assignment();
}

std::unique_ptr<Expr> Parser::assignment() {
    auto expr = logicalOr(); 

    if (match({ EQUAL })) {
        Token equals = previous();
        auto value = assignment();

        if (std::holds_alternative<Variable>(expr->as)) {
            Token name = std::get<Variable>(expr->as).name;
            return std::make_unique<Expr>(Assign{ name, std::move(value) });
        }

        throw error(equals, "Invalid assignment target.");
    }

    return expr;
}

std::unique_ptr<Expr> Parser::equality() {
    auto expr = comparison();

    while (match({ BANG_EQUAL, EQUAL_EQUAL })) {
        Token op = previous();
        auto right = comparison();
        expr = std::make_unique<Expr>(Binary{ std::move(expr), op, std::move(right) });
    }

    return expr;
}

std::unique_ptr<Expr> Parser::comparison() {
    auto expr = term();

    while (match({ GREATER, GREATER_EQUAL, LESS, LESS_EQUAL })) {
        Token op = previous();
        auto right = term();
        expr = std::make_unique<Expr>(Binary{ std::move(expr), op, std::move(right) });
    }

    return expr;
}

std::unique_ptr<Expr> Parser::term() {
    auto expr = factor();

    while (match({ MINUS, PLUS })) {
        Token op = previous();
        auto right = factor();
        expr = std::make_unique<Expr>(Binary{ std::move(expr), op, std::move(right) });
    }

    return expr;
}

std::unique_ptr<Expr> Parser::factor() {
    auto expr = unary();

    while (match({ SLASH, STAR })) {
        Token op = previous();
        auto right = unary();
        expr = std::make_unique<Expr>(Binary{ std::move(expr), op, std::move(right) });
    }

    return expr;
}

std::unique_ptr<Expr> Parser::unary() {
    if (match({ BANG, MINUS })) {
        Token op = previous();
        std::unique_ptr<Expr> right = unary();
        return std::make_unique<Expr>(Unary{ op, std::move(right) });
    }
    return call();
}

std::unique_ptr<Expr> Parser::call() {
    std::unique_ptr<Expr> expr = primary();

    // A function call can be chained, e.g., getFunction()(1)(2)
    while (true) {
        if (match({ LEFT_PAREN })) {
            expr = finishCall(std::move(expr));
        }
        else {
            break;
        }
    }

    return expr;
}

std::unique_ptr<Expr> Parser::finishCall(std::unique_ptr<Expr> callee) {
    std::vector<std::unique_ptr<Expr>> arguments;

    if (!check(RIGHT_PAREN)) {
        do {
            if (arguments.size() >= 255) {
                // report the error and keep parsing
                error(peek(), "Can't have more than 255 arguments.");
            }
            arguments.push_back(expression());
        } while (match({ COMMA }));
    }

    Token paren = consume(RIGHT_PAREN, "Expect ')' after arguments.");

    return std::make_unique<Expr>(Call{ std::move(callee), paren, std::move(arguments) });
}

std::unique_ptr<Stmt> Parser::function(const std::string& kind) {
    Token name = consume(IDENTIFIER, "Expect " + kind + " name.");

    consume(LEFT_PAREN, "Expect '(' after " + kind + " name.");
    std::vector<Token> parameters;

    if (!check(RIGHT_PAREN)) {
        do {
            if (parameters.size() >= 255) {
                error(peek(), "Can't have more than 255 parameters.");
            }
            parameters.push_back(consume(IDENTIFIER, "Expect parameter name."));
        } while (match({ COMMA }));
    }
    consume(RIGHT_PAREN, "Expect ')' after parameters.");

    consume(LEFT_BRACE, "Expect '{' before " + kind + " body.");
    std::vector<std::unique_ptr<Stmt>> body = block();

    return std::make_unique<Stmt>(FunctionStmt{ name, parameters, std::move(body) });
}

std::unique_ptr<Stmt> Parser::returnStatement() {
    Token keyword = previous();
    std::unique_ptr<Expr> value = nullptr;

    // If the next token isn't a semicolon, there must be a return value
    if (!check(SEMICOLON)) {
        value = expression();
    }

    consume(SEMICOLON, "Expect ';' after return value.");
    return std::make_unique<Stmt>(ReturnStmt{ keyword, std::move(value) });
}

std::unique_ptr<Expr> Parser::primary() {
    if (match({ FALSE })) return std::make_unique<Expr>(LiteralExpr{ false });
    if (match({ TRUE })) return std::make_unique<Expr>(LiteralExpr{ true });
    if (match({ NIL })) return std::make_unique<Expr>(LiteralExpr{ std::monostate{} });

    if (match({ NUMBER, STRING })) {
        return std::make_unique<Expr>(LiteralExpr{ previous().literal });
    }

    // Add the Variable identifier check
    if (match({ IDENTIFIER })) {
        return std::make_unique<Expr>(Variable{ previous() });
    }

    if (match({ LEFT_PAREN })) {
        auto expr = expression();
        consume(RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_unique<Expr>(Grouping{ std::move(expr) });
    }

    throw error(peek(), "Expect expression.");
}

// --- Control Flow Statements ---

std::unique_ptr<Stmt> Parser::ifStatement() {
    consume(LEFT_PAREN, "Expect '(' after 'if'.");
    auto condition = expression();
    consume(RIGHT_PAREN, "Expect ')' after if condition.");

    auto thenBranch = statement();
    std::unique_ptr<Stmt> elseBranch = nullptr;

    if (match({ ELSE })) {
        elseBranch = statement();
    }

    return std::make_unique<Stmt>(IfStmt{ std::move(condition), std::move(thenBranch), std::move(elseBranch) });
}

std::unique_ptr<Stmt> Parser::whileStatement() {
    consume(LEFT_PAREN, "Expect '(' after 'while'.");
    auto condition = expression();
    consume(RIGHT_PAREN, "Expect ')' after condition.");

    auto body = statement();

    return std::make_unique<Stmt>(WhileStmt{ std::move(condition), std::move(body) });
}

std::unique_ptr<Stmt> Parser::forStatement() {
    consume(LEFT_PAREN, "Expect '(' after 'for'.");

    // 1. Initializer
    std::unique_ptr<Stmt> initializer = nullptr;
    if (match({ SEMICOLON })) {
        initializer = nullptr;
    }
    else if (match({ VAR })) {
        initializer = varDeclaration();
    }
    else {
        initializer = expressionStatement();
    }

    // 2. Condition
    std::unique_ptr<Expr> condition = nullptr;
    if (!check(SEMICOLON)) {
        condition = expression();
    }
    consume(SEMICOLON, "Expect ';' after loop condition.");

    // 3. Increment
    std::unique_ptr<Expr> increment = nullptr;
    if (!check(RIGHT_PAREN)) {
        increment = expression();
    }
    consume(RIGHT_PAREN, "Expect ')' after for clauses.");

    auto body = statement();

    // Desugaring: Build the AST manually

    // If there is an increment, append it to the body block
    if (increment != nullptr) {
        std::vector<std::unique_ptr<Stmt>> stmts;
        stmts.push_back(std::move(body));
        stmts.push_back(std::make_unique<Stmt>(ExpressionStmt{ std::move(increment) }));
        body = std::make_unique<Stmt>(BlockStmt{ std::move(stmts) });
    }

    // Wrap the body in a while loop
    if (condition == nullptr) {
        condition = std::make_unique<Expr>(LiteralExpr{ true }); // Infinite loop if no condition
    }
    body = std::make_unique<Stmt>(WhileStmt{ std::move(condition), std::move(body) });

    // Wrap the initializer and the loop in a block
    if (initializer != nullptr) {
        std::vector<std::unique_ptr<Stmt>> stmts;
        stmts.push_back(std::move(initializer));
        stmts.push_back(std::move(body));
        body = std::make_unique<Stmt>(BlockStmt{ std::move(stmts) });
    }

    return body;
}

// --- Logical Expressions ---

std::unique_ptr<Expr> Parser::logicalOr() {
    auto expr = logicalAnd();

    while (match({ OR })) {
        Token op = previous();
        auto right = logicalAnd();
        expr = std::make_unique<Expr>(Logical{ std::move(expr), op, std::move(right) });
    }

    return expr;
}

std::unique_ptr<Expr> Parser::logicalAnd() {
    auto expr = equality();

    while (match({ AND })) {
        Token op = previous();
        auto right = equality();
        expr = std::make_unique<Expr>(Logical{ std::move(expr), op, std::move(right) });
    }

    return expr;
}

// --- Infrastructure ---

bool Parser::match(std::initializer_list<TokenType> types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::isAtEnd() {
    return peek().type == EOF_TOKEN;
}

Token Parser::peek() {
    return tokens[current];
}

Token Parser::previous() {
    return tokens[current - 1];
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw error(peek(), message);
}

ParseError Parser::error(Token token, const std::string& message) {
    caller->Report(token.line, message.c_str());
    return ParseError(message);
}

void Parser::synchronize() {
    advance();

    while (!isAtEnd()) {
        if (previous().type == SEMICOLON) return;

        switch (peek().type) {
        case CLASS: case FUN: case VAR: case FOR:
        case IF: case WHILE: case PRINT: case RETURN:
            return;
        default:
            break;
        }

        advance();
    }
}