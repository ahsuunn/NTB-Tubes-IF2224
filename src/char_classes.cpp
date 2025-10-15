#include "char_classes.hpp"
#include <cctype>

const std::unordered_set<std::string> KEYWORDS = {
    "program", "var", "begin", "end", "if", "then", "else", "while", "do", "for", "to", "downto",
    "integer", "real", "boolean", "char", "array", "of", "procedure", "function", "const", "type"
};

const std::unordered_set<std::string> LOGICAL_WORDS = {
    "and", "or", "not"
};

const std::unordered_set<std::string> ARITH_WORDS = {
    "div", "mod"
};

std::unordered_set<std::string> classify_char(char ch) {
    std::unordered_set<std::string> classes;

    if (ch == '\n')
        classes.insert("newline");
    if (ch == ' ' || ch == '\t' || ch == '\r')
        classes.insert("whitespace");
    if (std::isalpha(static_cast<unsigned char>(ch)))
        classes.insert("letter");
    if (std::isdigit(static_cast<unsigned char>(ch)))
        classes.insert("digit");
    if (ch == '_')
        classes.insert("underscore");

    const std::string syms = "+-*/=<>()[];,:'.";
    // note space before '.' to keep it inside the string literal
    if (syms.find(ch) != std::string::npos)
        classes.insert("symbol");

    return classes;
}