#pragma once
#include "dfa_loader.hpp"
#include "token.hpp"
#include "char_classes.hpp"
#include <string>
#include <vector>
#include <stdexcept>

class LexerError : public std::runtime_error
{
public:
    explicit LexerError(const std::string &msg) : std::runtime_error(msg) {}
};

class Lexer
{
public:
    Lexer(const DFA &dfa, std::string source)
        : dfa(dfa), src(std::move(source)), i(0), line(1), col(1) {}

    std::vector<Token> tokenize();

private:
    const DFA &dfa;
    std::string src;
    size_t i;
    int line, col;

    char peek(int k = 0) const;
    char advance();
    void skip_ws_comment(std::vector<std::string>& errors);
    std::string map_state_to_type(const std::string &state, const std::string &lex) const;
};
