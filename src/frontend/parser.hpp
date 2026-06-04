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