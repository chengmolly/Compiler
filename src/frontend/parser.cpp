#include "parser.hpp"
#include <stdexcept>
#include <sstream>

Parser::Parser(const std::vector<Token> &tokens) : tokens(tokens) {}

std::unique_ptr<Program> Parser::parse()
{
    return parseProgram();
}

const Token &Parser::peek() const
{
    return tokens[current];
}

const Token &Parser::advance()
{
    if (!isAtEnd())
    {
        current++;
    }
    return tokens[current - 1];
}

bool Parser::match(Token::Type type)
{
    if (check(type))
    {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(Token::Type type) const
{
    if (isAtEnd())
    {
        return false;
    }
    return peek().type == type;
}

bool Parser::isAtEnd() const
{
    return peek().type == Token::END_OF_FILE;
}

void Parser::consume(Token::Type type, const std::string &message)
{
    if (check(type))
    {
        advance();
        return;
    }
    std::stringstream ss;
    ss << "Error at line " << peek().line << ": " << message;
    throw std::runtime_error(ss.str());
}

std::unique_ptr<Program> Parser::parseProgram()
{
    std::vector<std::unique_ptr<FunctionDecl>> functions;
    while (!isAtEnd() && peek().type != Token::END_OF_FILE)
    {
        functions.push_back(parseFunction());
    }
    return std::make_unique<Program>(std::move(functions));
}

std::unique_ptr<FunctionDecl> Parser::parseFunction()
{
    consume(Token::Type::FUNCTION, "Expected 'function' keywords");

    std::string functionName = peek().lexeme;
    if (!match(Token::Type::IDENTIFIER))
    {
        throw std::runtime_error("Expected function name");
    }

    consume(Token::Type::LEFT_PAREN, "Expected '(' after function name");

    std::vector<Parameter> parameters;
    if (!check(Token::Type::RIGHT_PAREN))
    {
        parameters = parseParameterList();
    }

    consume(Token::Type::RIGHT_PAREN, "Expected ')' after parameter list");
    consume(Token::Type::COLON, "Expected ':' after parameter list");

    // Parse return type
    DataType returnType;
    if (match(Token::Type::INT))
    {
        returnType = DataType::INT;
    }
    else if (match(Token::Type::BOOL))
    {
        returnType = DataType::BOOL;
    }
    else
    {
        throw std::runtime_error("Expected return type (int or bool)");
    }

    // Parse function body
    auto body = parseBlock();

    return std::make_unique<FunctionDecl>(functionName, returnType, std::move(parameters), std::move(body));
}

std::vector<Parameter> Parser::parseParameterList()
{
    std::vector<Parameter> parameters;

    do
    {
        // Parse type
        DataType type;
        if (match(Token::Type::INT))
        {
            type = DataType::INT;
        }
        else if (match(Token::Type::BOOL))
        {
            type = DataType::BOOL;
        }
        else
        {
            throw std::runtime_error("Expected parameter type (int or bool)");
        }

        // Parse parameter name
        if (!match(Token::Type::IDENTIFIER))
        {
            throw std::runtime_error("Expected parameter name");
        }
        std::string name = tokens[current - 1].lexeme;
        parameters.push_back({type, name});

    } while (match(Token::Type::COMMA));

    return parameters;
}

std::unique_ptr<BlockStmt> Parser::parseBlock()
{
    consume(Token::Type::LEFT_BRACE, "Expected '{' after function body");

    std::vector<std::unique_ptr<Statement>> statements;
    while (!check(Token::Type::RIGHT_BRACE) && !isAtEnd())
    {
        statements.push_back(parseStatement());
    }

    consume(Token::Type::RIGHT_BRACE, "Expected '}' after function body");

    return std::make_unique<BlockStmt>(std::move(statements));
}

std::unique_ptr<Statement> Parser::parseStatement()
{
    if (match(Token::Type::IF))
    {
        return parseIfStmt();
    }
    if (match(Token::Type::WHILE))
    {
        return parseWhileStmt();
    }
    if (match(Token::Type::RETURN))
    {
        return parseReturnStmt();
    }
    if (check(Token::Type::INT) || check(Token::Type::BOOL))
    {
        return parseDeclaration();
    }
    if (check(Token::Type::LEFT_BRACE))
    {
        return parseBlock();
    }
    if (check(Token::Type::IDENTIFIER) &&
        (tokens[current + 1].type == Token::Type::ASSIGN ||
         tokens[current + 1].type == Token::Type::COMMA))
    {
        return parseAssignmentStmt();
    }

    return parseExprStmt();
}

std::unique_ptr<Statement> Parser::parseDeclaration()
{
    // Parse type
    DataType type;
    if (match(Token::Type::INT))
    {
        type = DataType::INT;
    }
    else if (match(Token::Type::BOOL))
    {
        type = DataType::BOOL;
    }
    else
    {
        throw std::runtime_error("Expected data type (int or bool)");
    }

    // Parse variable name
    std::vector<std::string> names;
    std::vector<std::unique_ptr<Expression>> initializers;

    do
    {
        if (!match(Token::Type::IDENTIFIER))
        {
            throw std::runtime_error("Expected variable name");
        }
        std::string name = tokens[current - 1].lexeme;
        names.push_back(name);

        // check for initializer
        if (match(Token::Type::ASSIGN))
        {
            initializers.push_back(parseExpression());
        }
        else
        {
            initializers.push_back(nullptr);
        }

    } while (match(Token::Type::COMMA));

    consume(Token::Type::SEMICOLON, "Expected ';' after variable declaration");

    auto varDecl = std::make_unique<VariableDecl>(type, names);

    // If any variables have initialization, wrap in a block with assignments
    bool hasInitialization = false;
    for (auto &init : initializers)
    {
        if (init != nullptr)
        {
            hasInitialization = true;
            break;
        }
    }
    if (!hasInitialization)
    {
        return varDecl;
    }

    // Create a block with declartion followed by assignments
    std::vector<std::unique_ptr<Statement>> statements;
    statements.push_back(std::move(varDecl));

    for (size_t i = 0; i < names.size(); ++i)
    {
        if (initializers[i] != nullptr)
        {
            statements.push_back(std::make_unique<AssignmentStmt>(names[i], std::move(initializers[i])));
        }
    }

    return std::make_unique<BlockStmt>(std::move(statements));
}

std::unique_ptr<Statement> Parser::parseIfStmt()
{
    consume(Token::Type::LEFT_PAREN, "Expected '(' after 'if'");
    auto condition = parseExpression();
    consume(Token::Type::RIGHT_PAREN, "Expected ')' after condition");

    auto thenStmt = parseStatement();

    std::unique_ptr<Statement> elseStmt = nullptr;
    if (match(Token::Type::ELSE))
    {
        elseStmt = parseStatement();
    }

    return std::make_unique<IfStmt>(std::move(condition), std::move(thenStmt), std::move(elseStmt));
}

std::unique_ptr<Statement> Parser::parseWhileStmt()
{
    consume(Token::Type::LEFT_PAREN, "Expected '(' after 'while'");
    auto condition = parseExpression();
    consume(Token::Type::RIGHT_PAREN, "Expected ')' after condition");

    auto body = parseStatement();

    return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
}

std::unique_ptr<Statement> Parser::parseReturnStmt()
{
    std::unique_ptr<Expression> expr = nullptr;
    if (!check(Token::Type::SEMICOLON))
    {
        expr = parseExpression();
    }
    consume(Token::Type::SEMICOLON, "Expected ';' after return value");

    return std::make_unique<ReturnStmt>(std::move(expr));
}

std::unique_ptr<Statement> Parser::parseAssignmentStmt()
{
    std::string varName = advance().lexeme;
    consume(Token::Type::ASSIGN, "Expected '=' after variable name");
    auto expr = parseExpression();
    consume(Token::Type::SEMICOLON, "Expected ';' after assignment");

    return std::make_unique<AssignmentStmt>(varName, std::move(expr));
}

std::unique_ptr<Statement> Parser::parseExprStmt()
{
    auto expr = parseExpression();
    consume(Token::Type::SEMICOLON, "Expected ';' after expression");
    return std::make_unique<ExprStmt>(std::move(expr));
}

std::unique_ptr<Expression> Parser::parseExpression()
{
    return parseLogicalOr();
}

std::unique_ptr<Expression> Parser::parseLogicalOr()
{
    auto expr = parseLogicalAnd();

    while (match(Token::Type::OR))
    {
        auto op = Token::Type::OR;
        auto right = parseLogicalAnd();
        expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
    }
    return expr;
}

std::unique_ptr<Expression> Parser::parseLogicalAnd()
{
    auto expr = parseEquality();

    while (match(Token::Type::AND))
    {
        auto op = Token::Type::AND;
        auto right = parseEquality();
        expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
    }
    return expr;
}

std::unique_ptr<Expression> Parser::parseEquality()
{
    auto expr = parseRelational();

    while (true)
    {
        if (match(Token::Type::EQUAL))
        {
            auto op = Token::Type::EQUAL;
            auto right = parseRelational();
            expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
        }
        else if (match(Token::Type::NOT_EQUAL))
        {
            auto op = Token::Type::NOT_EQUAL;
            auto right = parseRelational();
            expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
        }
        else
        {
            break;
        }
    }
    return expr;
}

std::unique_ptr<Expression> Parser::parseRelational()
{
    auto expr = parseAdditive();

    while (true)
    {
        if (match(Token::Type::LESS))
        {
            auto op = Token::Type::LESS;
            auto right = parseAdditive();
            expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
        }
        else if (match(Token::Type::LESS_EQUAL))
        {
            auto op = Token::Type::LESS_EQUAL;
            auto right = parseAdditive();
            expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
        }
        else if (match(Token::Type::GREATER))
        {
            auto op = Token::Type::GREATER;
            auto right = parseAdditive();
            expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
        }
        else if (match(Token::Type::GREATER_EQUAL))
        {
            auto op = Token::Type::GREATER_EQUAL;
            auto right = parseAdditive();
            expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
        }
        else
        {
            break;
        }
    }
    return expr;
}

std::unique_ptr<Expression> Parser::parseAdditive()
{
    auto expr = parseMultiplicative();

    while (true)
    {
        if (match(Token::Type::PLUS))
        {
            auto op = Token::Type::PLUS;
            auto right = parseMultiplicative();
            expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
        }
        else if (match(Token::Type::MINUS))
        {
            auto op = Token::Type::MINUS;
            auto right = parseMultiplicative();
            expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
        }
        else
        {
            break;
        }
    }
    return expr;
}

std::unique_ptr<Expression> Parser::parseMultiplicative()
{
    auto expr = parseUnary();

    while (true)
    {
        if (match(Token::Type::MULTIPLY))
        {
            auto op = Token::Type::MULTIPLY;
            auto right = parseUnary();
            expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
        }
        else if (match(Token::Type::DIVIDE))
        {
            auto op = Token::Type::DIVIDE;
            auto right = parseUnary();
            expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
        }
        else if (match(Token::Type::MODULO))
        {
            auto op = Token::Type::MODULO;
            auto right = parseUnary();
            expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
        }
        else
        {
            break;
        }
    }
    return expr;
}

std::unique_ptr<Expression> Parser::parseUnary()
{
    if (match(Token::Type::MINUS))
    {
        auto op = Token::Type::MINUS;
        auto right = parseUnary();
        return std::make_unique<UnaryExpr>(op, std::move(right));
    }
    if (match(Token::Type::NOT))
    {
        auto op = Token::Type::NOT;
        auto right = parseUnary();
        return std::make_unique<UnaryExpr>(op, std::move(right));
    }
    if (match(Token::Type::PLUS))
    {
        auto op = Token::Type::PLUS;
        auto right = parseUnary();
        return std::make_unique<UnaryExpr>(op, std::move(right));
    }
    return parsePrimary();
}

std::unique_ptr<Expression> Parser::parsePrimary()
{
    if (match(Token::Type::TRUE))
    {
        return std::make_unique<BooleanLiteral>(true);
    }
    if (match(Token::Type::FALSE))
    {
        return std::make_unique<BooleanLiteral>(false);
    }
    if (match(Token::Type::BOOLEAN_LITERAL))
    {
        bool value = tokens[current - 1].lexeme == "true";
        return std::make_unique<BooleanLiteral>(value);
    }

    if (match(Token::Type::INTEGER_LITERAL))
    {
        int value = std::stoi(tokens[current - 1].lexeme);
        return std::make_unique<IntegerLiteral>(value);
    }
    if (match(Token::Type::STRING_LITERAL))
    {
        return std::make_unique<StringLiteral>(tokens[current - 1].lexeme);
    }
    if (match(Token::Type::IDENTIFIER))
    {
        std::string name = tokens[current - 1].lexeme;

        // Check if it's a func call
        if (match(Token::Type::LEFT_PAREN))
        {
            std::vector<std::unique_ptr<Expression>> args;
            if (!check(Token::Type::RIGHT_PAREN))
            {
                args = parseArgumentList();
            }
            consume(Token::Type::RIGHT_PAREN, "Expected ')' after arguments");
            return std::make_unique<FunctionCallExpr>(name, std::move(args));
        }
        // otherwise it's a variable refer
        return std::make_unique<IdentifierExpr>(name);
    }

    if (match(Token::Type::LEFT_PAREN))
    {
        auto expr = parseExpression();
        consume(Token::Type::RIGHT_PAREN, "Expected ')' after expression");
        return expr;
    }
    throw std::runtime_error("Expected expression");
}
std::vector<std::unique_ptr<Expression>> Parser::parseArgumentList()
{
    std::vector<std::unique_ptr<Expression>> args;
    do
    {
        args.push_back(parseExpression());
    } while (match(Token::Type::COMMA));

    return args;
}