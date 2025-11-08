#include "lexer.hpp"
#include "parser.hpp"
#include "dfa_loader.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

static std::string resolve_from_here(const std::string& p) {
    fs::path cand(p);
    if (fs::exists(cand)) 
        return fs::weakly_canonical(cand).string();

    // try ../p
    fs::path alt = fs::path("..") / p;
    if (fs::exists(alt)) 
        return fs::weakly_canonical(alt).string();

    return p; // let open() fail later with clear error
}

// Helper function to print Parse Tree
void print_parse_tree(const ASTNode* node, const std::string& prefix = "", bool is_last = true) {
    if (!node) return;
    
    std::cout << prefix;
    std::cout << (is_last ? "└── " : "├── ");
    std::cout << node->toString() << "\n";
    
    std::string new_prefix = prefix + (is_last ? "    " : "│   ");
    
    auto children = node->getChildren();
    for (size_t i = 0; i < children.size(); i++) {
        print_parse_tree(children[i], new_prefix, i == children.size() - 1);
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <source.pas> [--dfa path/to/dfa.json|dfa.txt] [--tokens-only]\n";
        std::cerr << "\nOptions:\n";
        std::cerr << "  --dfa <path>      Specify DFA file (default: dfa/dfa.json)\n";
        std::cerr << "  --tokens-only     Only output tokens, skip parsing\n";
        return 1;
    }

    std::string source = argv[1];
    std::string dfa_path = "dfa/dfa.json";
    bool tokens_only = false;

    for (int i = 2; i < argc; i++) {
        std::string a = argv[i];
        if (a == "--dfa" && i + 1 < argc) {
            dfa_path = argv[++i];
        } else if (a == "--tokens-only") {
            tokens_only = true;
        }
    }

    source = resolve_from_here(source);
    dfa_path = resolve_from_here(dfa_path);

    // Load DFA
    DFA dfa;
    try {
        if (dfa_path.size() >= 4 && dfa_path.substr(dfa_path.size() - 4) == ".txt")
            dfa = load_dfa_txt(dfa_path);
        else if (dfa_path.size() >= 5 && dfa_path.substr(dfa_path.size() - 5) == ".json")
            dfa = load_dfa_json(dfa_path);
        else {
            std::cerr << "DFA must be .txt or .json\n";
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Failed to load DFA: " << e.what() << "\n";
        return 1;
    }

    // Read source file
    std::ifstream sf(source);
    if (!sf) {
        std::cerr << "Cannot open source: " << source << "\n";
        return 1;
    }

    std::string src((std::istreambuf_iterator<char>(sf)), std::istreambuf_iterator<char>());

    // Lexical Analysis
    std::vector<Token> tokens;
    try {
        Lexer lex(dfa, src);
        tokens = lex.tokenize();
        
        if (tokens_only) {
            // Only print tokens
            std::cout << "=== TOKENS ===\n";
            for (const auto& t : tokens) {
                std::cout << t.toString() << "\n";
            }
            return 0;
        }
        
        std::cout << "=== LEXICAL ANALYSIS SUCCESSFUL ===\n";
        std::cout << "Total tokens: " << tokens.size() << "\n\n";
        
    } catch (const LexerError& e) {
        std::cerr << "LEXER ERROR: " << e.what() << "\n";
        return 1;
        return 1;
    }

    // Syntax Analysis (Parsing)
    try {
        Parser parser(tokens);
        auto ast = parser.pars_program();
        
        std::cout << "=== PARSING SUCCESSFUL ===\n";
        std::cout << "Program name: " << ast->pars_program_name << "\n\n";
        
        std::cout << "=== PARSE TREE ===\n";
        print_parse_tree(ast.get());
        
    } catch (const std::exception& e) {
        std::cerr << "PARSER ERROR: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
