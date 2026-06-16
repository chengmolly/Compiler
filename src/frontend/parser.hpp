#ifndef PARAER_HPP
#define PARSER_HPP

#include <vector>
#include <string>
#include <memory>
#include <iostream>

struct Token
{
    enum Type
    {
        // Key Words
        FUNCTION,
        RETURN,
        IF,
        ELSE,
        WHILE,
        INT,
        BOOL,
        TRUE,
        FALSE,
        // Literals
        IDENTIFIER,
        INTEGER_LITERAL,
        BOOLEAN_LITERAL,
        STRING_LITERAL,
        // Opeartors
        PLUS,
        MINUS,
        MULTIPLY,
        DIVIDE,
        MODULO,
        EQUAL,
        NOT_EQUAL,
        LESS,
        LESS_EQUAL,
        GREATER,
        GREATER_EQUAL,
        AND,
        OR,
        NOT,
        ASSIGN,
        // Punctuators
        LEFT_PAREN,
        RIGHT_PAREN,
        LEFT_BRACE,
        RIGHT_BRACE,
        SEMICOLON,
        COMMA,
        COLON,
        // End of File
        END_OF_FILE,
        INVALID

    };

    Type type;
    std::string lexeme;
    size_t line;

    Token(Type type, const std::string &lexeme, size_t line)
        : type(type), lexeme(lexeme), line(line) {}
};

class ASTNode
{
public:
    virtual ~ASTNode() = default;
    virtual void print(size_t ident = 0) const = 0;
};

enum class DataType
{
    INT,
    STRING,
    BOOL
};

class Expression : public ASTNode
{
public:
    virtual ~Expression() = default;
};

class Statement : public ASTNode
{
public:
    virtual ~Statement() = default;
};

class IdentifierExpr : public Expression
{
public:
    std::string name;

    explicit IdentifierExpr(const std::string &name) : name(name) {}
    void print(size_t indent = 0) const override
    {
        std::cout << std::string(indent, ' ') << "IdentifierExpr: " << name << "\n";
    }
};

class IntegerLiteral : public Expression
{
public:
    size_t value;
    explicit IntegerLiteral(size_t value) : value(value) {}
    void print(size_t indent = 0) const override
    {
        std::cout << std::string(indent, ' ') << "IntegerLiteral: " << value << "\n";
    }
};

class BooleanLiteral : public Expression
{
public:
    bool value;
    explicit BooleanLiteral(bool value) : value(value) {}
    void print(size_t indent = 0) const override
    {
        std::cout << std::string(indent, ' ') << "BooleanLiteral: " << (value ? "true" : "false") << "\n";
    }
};

class StringLiteral : public Expression
{
public:
    std::string value;
    explicit StringLiteral(const std::string &value) : value(value) {}
    void print(size_t indent = 0) const override
    {
        std::cout << std::string(indent, ' ') << "StringLiteral: " << value << "\n";
    }
};

class BinaryExpr : public Expression
{
public:
    Token::Type op;
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
    BinaryExpr(Token::Type op, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
        : op(op), left(std::move(left)), right(std::move(right)) {}
    void print(size_t indent = 0) const override
    {
        std::cout << std::string(indent, ' ') << "BinaryExpr: ";
        switch (op)
        {
        case Token::Type::PLUS:
            std::cout << "+";
            break;
        case Token::Type::MINUS:
            std::cout << "-";
            break;
        case Token::Type::MULTIPLY:
            std::cout << "*";
            break;
        case Token::Type::DIVIDE:
            std::cout << "/";
            break;
        case Token::Type::MODULO:
            std::cout << "%";
            break;
        case Token::Type::EQUAL:
            std::cout << "==";
            break;
        case Token::Type::NOT_EQUAL:
            std::cout << "!=";
            break;
        case Token::Type::LESS:
            std::cout << "<";
            break;
        case Token::Type::LESS_EQUAL:
            std::cout << "<=";
            break;
        case Token::Type::GREATER:
            std::cout << ">";
            break;
        case Token::Type::GREATER_EQUAL:
            std::cout << ">=";
            break;
        case Token::Type::AND:
            std::cout << "&&";
            break;
        case Token::Type::OR:
            std::cout << "||";
            break;
        case Token::Type::NOT:
            std::cout << "!";
            break;
        case Token::Type::ASSIGN:
            std::cout << "=";
            break;
        default:
            std::cout << "UNKNOWN BinaryOP";
            break;
        }
        std::cout << "\n";
        left->print(indent + 2);
        right->print(indent + 2);
    }
};

class UnaryExpr : public Expression
{
public:
    Token::Type op;
    std::unique_ptr<Expression> expr;

    UnaryExpr(Token::Type op, std::unique_ptr<Expression> expr)
        : op(op), expr(std::move(expr)) {}

    void print(size_t indent = 0) const override
    {
        std::cout << std::string(indent, ' ') << "UnaryExpr: ";
        switch (op)
        {
        case Token::Type::MINUS:
            std::cout << "-";
            break;
        case Token::Type::NOT:
            std::cout << "!";
            break;
        default:
            std::cout << "UNKNOWN UnaryOP";
            break;
        }
        std::cout << "\n";
        expr->print(indent + 2);
    }
};

class FunctionCallExpr : public Expression
{
public:
    std::string functionName;
    std::vector<std::unique_ptr<Expression>> arguments;

    FunctionCallExpr(const std::string &functionName, std::vector<std::unique_ptr<Expression>> arguments)
        : functionName(functionName), arguments(std::move(arguments)) {}

    void print(size_t indent = 0) const override
    {
        std::cout << std::string(indent, ' ') << "FunctionCallExpr: " << functionName << "\n";
        for (const auto &arg : arguments)
        {
            arg->print(indent + 2);
        }
    }
};

class VariableDecl : public Statement
{
public:
    DataType type;
    std::vector<std::string> names;

    VariableDecl(DataType type, std::vector<std::string> names)
        : type(type), names(std::move(names)) {}

    void print(size_t indent = 0) const override
    {
        std::cout << std::string(indent, ' ') << "VariableDecl: ";
        switch (type)
        {
        case DataType::INT:
            std::cout << "int";
            break;
        case DataType::BOOL:
            std::cout << "bool";
            break;
        case DataType::STRING:
            std::cout << "string";
            break;
        default:
            std::cout << "UNKNOWN DataType";
            break;
        }
        std::cout << "\n";
        for (const auto &name : names)
        {
            std::cout << std::string(indent + 2, ' ') << name << "\n";
        }
    }
};

class AssignmentStmt : public Statement
{
public:
    std::string variableName;
    std::unique_ptr<Expression> expr;

    AssignmentStmt(const std::string &variableName, std::unique_ptr<Expression> expr)
        : variableName(variableName), expr(std::move(expr)) {}

    void print(size_t indent = 0) const override
    {
        std::cout << std::string(indent, ' ') << "AssignmentStmt: " << variableName << "\n";
        expr->print(indent + 2);
    }
};

class ExprStmt : public Statement
{
public:
    std::unique_ptr<Expression> expr;

    explicit ExprStmt(std::unique_ptr<Expression> expr) : expr(std::move(expr)) {}

    void print(size_t indent = 0) const override
    {
        std::cout << std::string(indent, ' ') << "ExprStmt: \n";
        expr->print(indent + 2);
    }
};

class BlockStmt : public Statement
{
public:
    std::vector<std::unique_ptr<Statement>> statements;

    BlockStmt(std::vector<std::unique_ptr<Statement>> statements)
        : statements(std::move(statements)) {}

    void print(size_t indent = 0) const override
    {
        std::cout << std::string(indent, ' ') << "BlockStmt: " << "\n";
        for (const auto &stmt : statements)
        {
            stmt->print(indent + 2);
        }
    }
};

class IfStmt : public Statement
{
public:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> thenStmt;
    std::unique_ptr<Statement> elseStmt;

    IfStmt(std::unique_ptr<Expression> condition,
           std::unique_ptr<Statement> thenStmt,
           std::unique_ptr<Statement> elseStmt)
        : condition(std::move(condition)),
          thenStmt(std::move(thenStmt)),
          elseStmt(std::move(elseStmt)) {}

    void print(size_t indent = 0) const override
    {
        std::cout << std::string(indent, ' ') << "IfStmt: " << "\n";
        condition->print(indent + 2);
        thenStmt->print(indent + 2);
        if (elseStmt)
        {
            elseStmt->print(indent + 2);
        }
    }
};

class WhileStmt : public Statement
{
public:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> body;

    WhileStmt(std::unique_ptr<Expression> condition, std::unique_ptr<Statement> body)
        : condition(std::move(condition)), body(std::move(body)) {}

    void print(size_t indent = 0) const override
    {
        std::cout << std::string(indent, ' ') << "WhileStmt: " << "\n";
        condition->print(indent + 2);
        body->print(indent + 2);
    }
};

class ReturnStmt : public Statement
{
public:
    std::unique_ptr<Expression> expr;
    ReturnStmt(std::unique_ptr<Expression> expr)
        : expr(std::move(expr)) {}

    void print(size_t indent = 0) const override
    {
        std::cout << std::string(indent, ' ') << "ReturnStmt: " << "\n";
        expr->print(indent + 2);
    }
};

struct Parameter
{
    DataType type;
    std::string name;
};

class FunctionDecl : public ASTNode
{
public:
    std::string name;
    DataType returnType;
    std::vector<Parameter> parameters;
    std::unique_ptr<BlockStmt> body;
    FunctionDecl(const std::string &name, DataType returnType,
                 const std::vector<Parameter> &parameters,
                 std::unique_ptr<BlockStmt> body)
        : name(name), returnType(returnType),
          parameters(parameters), body(std::move(body)) {}

    void print(size_t indent = 0) const override
    {
        std::cout << std::string(indent, ' ') << "FunctionDecl: " << name << "\n";
        std::cout << std::string(indent + 2, ' ') << "ReturnType: ";
        switch (returnType)
        {
        case DataType::INT:
            std::cout << "int";
            break;
        case DataType::BOOL:
            std::cout << "bool";
            break;
        case DataType::STRING:
            std::cout << "string";
            break;
        default:
            std::cout << "Unknown Type";
            break;
        }
        std::cout << "\n";
        for (const auto &param : parameters)
        {
            std::cout << std::string(indent + 2, ' ') << "Parameter: ";
            switch (param.type)
            {
            case DataType::INT:
                std::cout << "int";
                break;
            case DataType::BOOL:
                std::cout << "bool";
                break;
            case DataType::STRING:
                std::cout << "string";
                break;
            default:
                std::cout << "Unknown Type";
                break;
            }
            std::cout << " " << param.name << "\n";
        }
        body->print(indent + 2);
    }
};

class Program : public ASTNode
{
public:
    std::vector<std::unique_ptr<FunctionDecl>> functions;

    Program(std::vector<std::unique_ptr<FunctionDecl>> functions) : functions(std::move(functions)) {}

    void print(size_t indent = 0) const override
    {
        std::cout << std::string(indent, ' ') << "Program: " << "\n";
        for (const auto &func : functions)
        {
            func->print(indent + 2);
        }
    }
};

class Parser
{
public:
    explicit Parser(const std::vector<Token> &tokens);

    std::unique_ptr<Program> parse();

private:
    const Token &peek() const;
    const Token &advance();
    bool match(Token::Type type);
    bool check(Token::Type type) const;
    bool isAtEnd() const;
    void consume(Token::Type type, const std::string &message);

    std::unique_ptr<Program> parseProgram();
    std::unique_ptr<FunctionDecl> parseFunction();
    std::vector<Parameter> parseParameterList();
    std::unique_ptr<BlockStmt> parseBlock();
    std::unique_ptr<Statement> parseStatement();
    std::unique_ptr<Statement> parseDeclaration();
    std::unique_ptr<Statement> parseIfStmt();
    std::unique_ptr<Statement> parseWhileStmt();
    std::unique_ptr<Statement> parseReturnStmt();
    std::unique_ptr<Statement> parseAssignmentStmt();
    std::unique_ptr<Statement> parseExprStmt();
    std::unique_ptr<Expression> parseExpression();
    std::unique_ptr<Expression> parseLogicalOr();
    std::unique_ptr<Expression> parseLogicalAnd();
    std::unique_ptr<Expression> parseEquality();
    std::unique_ptr<Expression> parseRelational();
    std::unique_ptr<Expression> parseAdditive();
    std::unique_ptr<Expression> parseMultiplicative();
    std::unique_ptr<Expression> parseUnary();
    std::unique_ptr<Expression> parseFunctionCall();
    std::unique_ptr<Expression> parsePrimary();
    std::vector<std::unique_ptr<Expression>> parseArgumentList();

    const std::vector<Token> &tokens;
    size_t current = 0;
};

#endif