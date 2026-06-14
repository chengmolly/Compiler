#include "error.hpp"
#include <sstream>

namespace compiler
{
    static std::string formatMessage(const std::string &message, size_t line, size_t column)
    {
        std::stringstream ss;
        ss << "Lexer error at line " << line << ", column " << column << ": " << message;
        return ss.str();
    }

    static std::string formatMessage(const std::string &message, size_t line)
    {
        std::stringstream ss;
        ss << "Parser error at line " << line << ": " << message;
        return ss.str();
    }

    CompilerError::CompilerError(const std::string &message) : std::runtime_error(message) {}
    CompilerError::CompilerError(const char *message) : CompilerError(std::string(message)) {}

    LexerError::LexerError(const std::string &message, size_t line, size_t column)
        : CompilerError(formatMessage(message, line, column)), line_(line), column_(column) {}

    ParserError::ParserError(const std::string &message, size_t line)
        : CompilerError(formatMessage(message, line)), line_(line) {}

    SemanticError::SemanticError(const std::string &message, size_t line)
        : CompilerError(message), line_(line) {}

}