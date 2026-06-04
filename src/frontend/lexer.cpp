#include "lexer.hpp"
#include <unordered_map>

namespace frontend
{
    Lexer::Lexer(const std::string &source)
        : source_(source), start_(0), current_(0), line_(1), column_(1), startColumn_(1) {}
} // namespace frontend