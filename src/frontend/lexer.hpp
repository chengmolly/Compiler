#ifndef LEXER_HPP
#define LEXER_HPP

#include "parser.hpp"
#include "../utils/error.hpp"
#include <string>
#include <vector>
#include <memory>

namespace frontend
{

    class Lexer
    {
    public:
        explicit Lexer(const std::string &source);

        std::vector<Token> tokenize();

        int getCurrentLine() const { return line_; }
        int getCurrentColumn() const { return column_; }

    private:
        char peek() const;
        char peekNext() const;
        char advance();
        bool isAtEnd() const;

        Token makeToken(Token::Type type) const;
        Token makeToken(Token::Type type, const std::string &lexeme) const;

        Token scanToken();
        Token scanString();
        Token scanNumber();
        Token scanIdentifier();
        Token scanComment();

        bool isDigit(char c) const;
        ;
        bool isAlpha(char c) const;
        bool isAlphaNumeric(char c) const;

        // Keyword recognition
        Token::Type getIdentifierType(const std::string &text) const;
        // Skip whitespace and comments
        void skipWhitespace();
        // Error handling
        void reportError(const std::string &message) const;

    private:
        const std::string &source_;
        std::vector<Token> tokens_;

        size_t start_;       // start of current token
        size_t current_;     // current character
        size_t line_;        // current line number
        size_t column_;      // current column number
        size_t startColumn_; // column number of start of current token
    };
} // namespace frontend

#endif