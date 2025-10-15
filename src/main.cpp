#include "lexer.hpp"
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

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: paslex <source.pas> [--dfa path/to/dfa.json|dfa.txt]\n";
        return 1;
    }

    std::string source = argv[1];
    std::string dfa_path = "dfa/dfa.json";

    for (int i = 2; i < argc; i++) {
        std::string a = argv[i];
        if (a == "--dfa" && i + 1 < argc) {
            dfa_path = argv[++i];
        }
    }

    source = resolve_from_here(source);
    dfa_path = resolve_from_here(dfa_path);

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

    std::ifstream sf(source);
    if (!sf) {
        std::cerr << "Cannot open source: " << source << "\n";
        return 1;
    }

    std::string src((std::istreambuf_iterator<char>(sf)), std::istreambuf_iterator<char>());

    try {
        Lexer lex(dfa, src);
        auto tokens = lex.tokenize();
        for (const auto& t : tokens) {
            std::cout << t.toString() << "\n";
        }
    } catch (const LexerError& e) {
        std::cerr << "LEXER ERROR: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
