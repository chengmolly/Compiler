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
};

#endif