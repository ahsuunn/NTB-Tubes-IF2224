#pragma once
#include <string>
#include <unordered_set>

extern const std::unordered_set<std::string> KEYWORDS;
extern const std::unordered_set<std::string> LOGICAL_WORDS;

// Return a set of class labels for a single character
std::unordered_set<std::string> classify_char(char ch);
