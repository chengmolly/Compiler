#ifndef ERROR_HPP
#define ERROR_HPP

#include <stdexcept>
#include <string>

namespace compiler
{
    class CompilerError : public std::runtime_error
    {
    public:
        explicit CompilerError(const std::string &message);
        explicit CompilerError(const char *message);
    };

    class LexerError : public CompilerError
    {
    public:
        LexerError(const std::string &message, size_t line, size_t column);
        size_t getLine() const { return line_; }
        size_t getColumn() const { return column_; }

    private:
        size_t line_;
        size_t column_;
    };

    class ParserError : public CompilerError
    {
    public:
        ParserError(const std::string &message, size_t line);
        size_t getLine() const { return line_; }

    private:
        size_t line_;
    };

    class SemanticError : public CompilerError
    {
    public:
        SemanticError(const std::string &message, size_t line);
        size_t getLine() const { return line_; }

    private:
        size_t line_;
    };
} // namespace compiler

#endif
