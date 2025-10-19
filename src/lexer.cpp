// src/lexer.cpp
#include "lexer.hpp"
#include <sstream>
#include <cctype>
#include <unordered_map>

#include <sstream>
#include <cctype>

char Lexer::peek(int k) const {
    size_t idx = i + (k < 0 ? 0 : static_cast<size_t>(k));
    if (idx < src.size()) return src[idx];
    return 0; // acts like None
}

char Lexer::advance() {
    if (i >= src.size()) return 0;
    char ch = src[i++];
    if (ch == '\n') { line++; col = 1; }
    else { col++; }
    return ch;
}

void Lexer::skip_ws_comment(std::vector<std::string>& errors) {
    while (true) {
        char ch = peek();
        if (!ch) return;

        // whitespace
        if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n') {
            advance();
            continue;
        }
        // { ... } comment
        if (ch == '{') {
            int start_line = line, start_col = col;
            advance();
            bool terminated = false;
            while (true) {
                char n = advance();
                if (!n) { 
                    std::ostringstream oss;
                    oss << "Unterminated { ... } comment at line " << start_line << ", col " << start_col; 
                    errors.push_back(oss.str());
                    break;  // Exit loop and continue lexing
                } 
                if (n == '}') {
                    terminated = true;
                    break;
                }
            }
            continue;
        }
        // (* ... *) comment
        if (ch == '(' && peek(1) == '*') {
            int start_line = line, start_col = col;
            advance(); advance(); // consume (*
            bool terminated = false;
            while (true) {
                char n = advance();
                if (!n) {
                    std::ostringstream oss;
                    oss << "Unterminated (* ... *) comment at line " << start_line << ", col " << start_col; 
                    errors.push_back(oss.str());
                    break;  // Exit loop and continue lexing
                }
                if (n == '*' && peek() == ')') { 
                    advance(); 
                    terminated = true;
                    break; 
                }
            }
            continue;
        }
        break;
    }
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    std::vector<std::string> errors;

    while (true) {
        skip_ws_comment(errors);
        char ch = peek();
        if (!ch) break;

        int start_line = line, start_col = col;
        std::string state = dfa.start();
        std::string lexeme;
        std::string last_final_state;
        std::string last_final_lexeme;

        size_t save_i = i; int save_line = line, save_col = col;

        auto trans = [&](const std::string& curr_state, char c) -> std::string {
            // 1) exact char
            {
                std::string s(1, c);
                auto nxt = dfa.next_state(curr_state, s);
                if (!nxt.empty()) return nxt;
            }
            // 2) char classes
            for (const auto& cls : classify_char(c)) {
                auto nxt = dfa.next_state(curr_state, cls);
                if (!nxt.empty()) return nxt;
            }
            // 3) alnum or underscore
            if (std::isalnum(static_cast<unsigned char>(c)) || c == '_') {
                auto nxt = dfa.next_state(curr_state, "letter_or_digit_or_underscore");
                if (!nxt.empty()) return nxt;
            }
            // 4) wildcard any
            {
                auto nxt = dfa.next_state(curr_state, "any");
                if (!nxt.empty()) return nxt;
            }
            // any_non_quote untuk di dalam string/char
            if (c != '\'') {
                auto nxt2 = dfa.next_state(curr_state, "any_non_quote");
                if (!nxt2.empty()) return nxt2;
            }
            return std::string();
        };

        // walk DFA
        while (true) {
            char c = peek();
            if (!c) break;
            std::string nxt = trans(state, c);
            if (nxt.empty()) break;
            lexeme.push_back(advance());
            state = nxt;
            if (dfa.finals().count(state)) {
                last_final_state = state;
                last_final_lexeme = lexeme;
            }
        }

        if (last_final_state.empty()) {
            char bad = peek();
            std::ostringstream oss;
            oss << "Unexpected character '" << (bad ? bad : '?')
                << "' at line " << start_line << ", col " << start_col;
            errors.push_back(oss.str());
            // Skip the bad character and continue
            if(peek()) advance();
            continue;
        }

        // rollback then re-consume exact final lexeme
        i = save_i; line = save_line; col = save_col; lexeme.clear();
        for (size_t k = 0; k < last_final_lexeme.size(); ++k) lexeme.push_back(advance());

        std::string tok_type = map_state_to_type(last_final_state, lexeme);
        if (tok_type == "IDENTIFIER") {
            std::string lw = lexeme;
            for (char& c : lw) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            if (KEYWORDS.count(lw)) tok_type = "KEYWORD";
            else if (LOGICAL_WORDS.count(lw)) tok_type = "LOGICAL_OPERATOR";
            else if (ARITH_WORDS.count(lw)) tok_type = "ARITHMETIC_OPERATOR";
        }

        tokens.push_back(Token{tok_type, lexeme, start_line, start_col});
    }
    
    // Display all collected errors at the end
    if (!errors.empty()) {
        std::ostringstream agg;
        agg << "Lexical errors found (" << errors.size() << "):\n";
        for (size_t idx = 0; idx < errors.size(); ++idx) {
            agg << "  [" << (idx + 1) << "] " << errors[idx] << "\n";
        }
        throw LexerError(agg.str());
    }
    
    return tokens;
}

std::string Lexer::map_state_to_type(const std::string& state, const std::string& lex) const {
    // numbers
    if (state == "NUM_INT" || state == "NUM_REAL") return "NUMBER";

    // range & assign
    if (lex == ":=") return "ASSIGN_OPERATOR";
    if (lex == "..") return "RANGE_OPERATOR";

    // string/char
    if (state == "STR") return "STRING_LITERAL";
    if (state == "CHR") return "CHAR_LITERAL";

    // punctuation (spesifik nama)
    if (lex == ";") return "SEMICOLON";
    if (lex == ",") return "COMMA";
    if (lex == ":") return "COLON";
    if (lex == ".") return "DOT";
    if (lex == "(") return "LPARENTHESIS";
    if (lex == ")") return "RPARENTHESIS";
    if (lex == "[") return "LBRACKET";
    if (lex == "]") return "RBRACKET";

    // operators
    if (lex == "+" || lex == "-" || lex == "*" || lex == "/")
        return "ARITHMETIC_OPERATOR";
    if (lex == "=" || lex == "<>" || lex == "<" || lex == "<=" || lex == ">" || lex == ">=")
        return "RELATIONAL_OPERATOR";

    // identifier / keyword / logical word handled di atas (post-processing)
    if (state == "ID") return "IDENTIFIER";

    // fallback ke nama state (aman)
    return state;
}