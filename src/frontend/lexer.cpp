#include "lexer.hpp"
#include <unordered_map>

namespace frontend
{
    Lexer::Lexer(const std::string &source)
        : source_(source), start_(0), current_(0), line_(1), column_(1), startColumn_(1) {}

    std::vector<Token> Lexer::tokenize()
    {
        tokens_.clear();

        while (!isAtEnd())
        {
            start_ = current_;
            startColumn_ = column_;

            try
            {
                Token token = scanToken();
                if (token.type != Token::Type::INVALID)
                {
                    tokens_.push_back(token);
                }
            }
            catch (const std::exception &e)
            {
                reportError(e.what());
                advance();
            }
        }
        // Add EOF token
        tokens_.push_back(Token(Token::Type::END_OF_FILE, "", line_));
        return tokens_;
    }

    char Lexer::peek() const
    {
        if (isAtEnd())
            return '\0';
        return source_[current_];
    }

    char Lexer::peekNext() const
    {
        if (current_ + 1 >= source_.length())
            return '\0';
        return source_[current_ + 1];
    }

    char Lexer::advance()
    {
        if (isAtEnd())
            return '\0';

        char c = source_[current_++];
        if (c == '\n')
        {
            line_++;
            column_ = 1;
        }
        else
        {
            column_++;
        }
        return c;
    }

    bool Lexer::isAtEnd() const
    {
        return current_ >= source_.length();
    }

    Token Lexer::makeToken(Token::Type type) const
    {
        std::string lexeme = source_.substr(start_, current_ - start_);
        return Token(type, lexeme, line_);
    }

    Token Lexer::makeToken(Token::Type type, const std::string &lexeme) const
    {
        return Token(type, lexeme, line_);
    }

    Token Lexer::scanToken()
    {
        skipWhitespace();

        start_ = current_;
        startColumn_ = column_;

        if (isAtEnd())
        {
            return makeToken(Token::Type::END_OF_FILE);
        }

        char c = advance();

        switch (c)
        {
        case '(':
            return makeToken(Token::Type::LEFT_PAREN);
        case ')':
            return makeToken(Token::Type::RIGHT_PAREN);
        case '{':
            return makeToken(Token::Type::LEFT_BRACE);
        case '}':
            return makeToken(Token::Type::RIGHT_BRACE);
        case ';':
            return makeToken(Token::Type::SEMICOLON);
        case ',':
            return makeToken(Token::Type::COMMA);
        case ':':
            return makeToken(Token::Type::COLON);
        case '+':
            return makeToken(Token::Type::PLUS);
        case '-':
            return makeToken(Token::Type::MINUS);
        case '*':
            return makeToken(Token::Type::MULTIPLY);
        case '%':
            return makeToken(Token::Type::MODULO);

        case '=':
            if (peek() == '=')
            {
                advance();
                return makeToken(Token::Type::EQUAL);
            }
            return makeToken(Token::Type::ASSIGN);

        case '!':
            if (peek() == '=')
            {
                advance();
                return makeToken(Token::Type::NOT_EQUAL);
            }
            return makeToken(Token::Type::NOT);

        case '<':
            if (peek() == '=')
            {
                advance();
                return makeToken(Token::Type::LESS_EQUAL);
            }
            return makeToken(Token::Type::LESS);

        case '>':
            if (peek() == '=')
            {
                advance();
                return makeToken(Token::Type::GREATER_EQUAL);
            }
            return makeToken(Token::Type::GREATER);

        case '&':
            if (peek() == '&')
            {
                advance();
                return makeToken(Token::Type::AND);
            }
            reportError("Unexpected character '&'. Did you mean '&&'?");
            return makeToken(Token::Type::INVALID);

        case '|':
            if (peek() == '|')
            {
                advance();
                return makeToken(Token::Type::OR);
            }
            reportError("Unexpected character '|'. Did you mean '||'?");
            return makeToken(Token::Type::INVALID);

        case '/':
            if (peek() == '/')
            {
                return scanComment();
            }
            else if (peek() == '*')
            {
                advance();
                return scanComment();
            }
            return makeToken(Token::Type::DIVIDE);

        case '"':
            return scanString();

        default:
            if (isDigit(c))
            {
                return scanNumber();
            }
            if (isAlpha(c))
            {
                return scanIdentifier();
            }
            reportError("Unexpected character: " + std::string(1, c) + "'");
            return makeToken(Token::Type::INVALID);
        }
    }

    Token Lexer::scanString()
    {
        std::string value;

        while (peek() != '"' && !isAtEnd())
        {
            if (peek() == '\n')
            {
                reportError("Unterminated string literal");
                return makeToken(Token::Type::INVALID);
            }

            char c = advance();

            if (c == '\\')
            {
                if (isAtEnd())
                {
                    reportError("Unterminated string literal");
                    return makeToken(Token::Type::INVALID);
                }

                char escaped = advance();
                switch (escaped)
                {
                case 'n':
                    value += '\n';
                    break;
                case 'r':
                    value += '\r';
                    break;
                case 't':
                    value += '\t';
                    break;
                case '\\':
                    value += '\\';
                    break;
                case '"':
                    value += '"';
                    break;
                default:
                    reportError("Invalid escape sequence '\\" + std::string(1, escaped) + "'");
                    return makeToken(Token::Type::INVALID);
                }
            }
            else
            {
                value += c;
            }
        }
        if (isAtEnd())
        {
            reportError("Unterminated string literal");
            return makeToken(Token::Type::INVALID);
        }

        advance();
        return makeToken(Token::Type::STRING_LITERAL, value);
    }

    Token Lexer::scanNumber()
    {
        while (isDigit(peek()))
        {
            advance();
        }
        std::string lexeme = source_.substr(start_, current_ - start_);
        return makeToken(Token::Type::INTEGER_LITERAL, lexeme);
    }

    Token Lexer::scanIdentifier()
    {
        while (isAlphaNumeric(peek()))
        {
            advance();
        }
        std::string lexeme = source_.substr(start_, current_ - start_);
        Token::Type type = getIdentifierType(lexeme);
        return makeToken(type, lexeme);
    }

    Token Lexer::scanComment()
    {
        if (source_[current_ - 1] == '/' && peek() == '/')
        {
            advance();
            while (peek() != '\n' && !isAtEnd())
            {
                advance();
            }
        }
        else if (source_[current_ - 1] == '*')
        {
            while (!isAtEnd())
            {
                if (peek() == '*' && peekNext() == '/')
                {
                    advance();
                    advance();
                    break;
                }
                advance();
            }
            if (isAtEnd())
            {
                reportError("Unterminated multi-line comment");
                return makeToken(Token::Type::INVALID);
            }
        }
        return makeToken(Token::Type::INVALID);
    }

    bool Lexer::isDigit(char c) const
    {
        return c >= '0' && c <= '9';
    }

    bool Lexer::isAlpha(char c) const
    {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
    }

    bool Lexer::isAlphaNumeric(char c) const
    {
        return isAlpha(c) || isDigit(c);
    }

    Token::Type Lexer::getIdentifierType(const std::string &text) const
    {
        static const std::unordered_map<std::string, Token::Type> keywords = {
            {"function", Token::Type::FUNCTION},
            {"return", Token::Type::RETURN},
            {"if", Token::Type::IF},
            {"else", Token::Type::ELSE},
            {"while", Token::Type::WHILE},
            {"int", Token::Type::INT},
            {"bool", Token::Type::BOOL},
            {"true", Token::Type::BOOLEAN_LITERAL},
            {"false", Token::Type::BOOLEAN_LITERAL}};

        auto it = keywords.find(text);
        if (it != keywords.end())
        {
            return it->second;
        }
        return Token::Type::IDENTIFIER;
    }

    void Lexer::skipWhitespace()
    {
        while (!isAtEnd())
        {
            char c = peek();
            if (c == ' ' || c == '\r' || c == '\t' || c == '\n')
            {
                advance();
            }
            else
            {
                break;
            }
        }
    }

    void Lexer::reportError(const std::string &message) const
    {
        throw compiler::LexerError(message, line_, startColumn_);
    }
} // namespace frontend