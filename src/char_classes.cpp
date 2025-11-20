#include "char_classes.hpp"
#include <cctype>

const std::unordered_set<std::string> KEYWORDS = {
    "program", "konstanta", "tipe", "variabel", "prosedur", "fungsi",
    "mulai", "selesai", "jika", "maka", "selain-itu", "selama", "lakukan",
    "untuk", "ke", "turun-ke", "ulangi", "sampai", "kasus", "dari",
    "larik", "rekaman",
    "integer", "real", "boolean", "char", "string",
    "true", "false"
};

const std::unordered_set<std::string> LOGICAL_WORDS = {
    "dan", "atau", "tidak"
};

const std::unordered_set<std::string> ARITH_WORDS = {
    "bagi", "mod"
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
    if (ch == '-')
        classes.insert("dash");

    const std::string syms = "+-*/=<>()[];,:'.";
    
    if (syms.find(ch) != std::string::npos)
        classes.insert("symbol");

    return classes;
}