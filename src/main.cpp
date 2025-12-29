#include "utils.hpp"
#include "lexer/lexer.hpp"
#include "lexer/dfa_loader.hpp"
#include "parser/parser.hpp"
#include "semantic/scope_type_checker.hpp"
#include "ast/ast_builder.hpp"
#include "ast/ast_printer.hpp"
#include "ast/ast_printer_decorated.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>


int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <source.pas> [--dfa path/to/dfa.json|dfa.txt] [--tokens-only] [--ast]\n";
        std::cerr << "\nOptions:\n";
        std::cerr << "  --dfa <path>      Specify DFA file (default: dfa/dfa.json)\n";
        std::cerr << "  --tokens-only     Only output tokens, skip parsing\n";
        std::cerr << "  --ast             Build and print Abstract Syntax Tree\n";
        return 1;
    }

    std::string source = argv[1];
    std::string dfa_path = "dfa/dfa.json";
    bool tokens_only = false;
    bool build_ast = false;
    bool decorated = false; 

    for (int i = 2; i < argc; i++) {
        std::string a = argv[i];
        if (a == "--dfa" && i + 1 < argc) {
            dfa_path = argv[++i];
        } else if (a == "--tokens-only") {
            tokens_only = true;
        } else if (a == "--ast") {
            build_ast = true; 
        } else if (a == "--decorated") {
            decorated = true;
            build_ast = true;
        }
    }

    source = Utils::resolve_from_here(source);
    dfa_path = Utils::resolve_from_here(dfa_path);

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

    try {
        Parser parser(tokens);
        auto parsetree = parser.pars_program();
        
        std::cout << "=== PARSING SUCCESSFUL ===\n";
        std::cout << "Program name: " << parsetree->pars_program_name << "\n\n";
        
        std::cout << "=== PARSE TREE ===\n";
        Utils::print_parse_tree(parsetree.get());
        
        // Semantic Analysis (Scope and Type Checking)
        std::cout << "\n=== SEMANTIC ANALYSIS ===\n";
        SymbolTable symTab;
        try {
            ScopeTypeChecker checker(&symTab);
            checker.visitProgram(parsetree.get());
            
            std::cout << "\n=== SEMANTIC ANALYSIS SUCCESSFUL ===\n";
            std::cout << "\n=== SYMBOL TABLE ===\n";
            symTab.print_tab();
            std::cout << "\n=== BLOCK TABLE ===\n";
            symTab.print_btab();
            if (symTab.get_atab_size() > 0) {
                std::cout << "\n=== ARRAY TABLE ===\n";
                symTab.print_atab();
            }
            
        } catch (const SemanticError& e) {
            std::cerr << "\nSEMANTIC ERROR: " << e.what() << "\n";
            return 1;
        } catch (const std::exception& e) {
            std::cerr << "\nSEMANTIC ANALYSIS ERROR: " << e.what() << "\n";
            return 1;
        }
        
        // build AST dari parse tree
        if (build_ast) {
            std::cout << "\n=== BUILDING AST ===\n";
            try {
                ASTBuilder builder;
                auto ast = builder.buildAST(parsetree.get());
                
                std::cout << "=== AST BUILT SUCCESSFULLY ===\n\n";
                
                if (decorated) {
                    // Print decorated AST
                    std::cout << "=== DECORATED AST ===\n";
                    ASTDecoratedPrinter decoratedPrinter(&symTab);
                    ast->accept(&decoratedPrinter);
                } else {
                    std::cout << "=== ABSTRACT SYNTAX TREE ===\n";
                    ASTPrinter printer;
                    ast->accept(&printer);
                }
                
            } catch (const std::exception& e) {
                std::cerr << "AST BUILD ERROR: " << e.what() << "\n";
                return 1;
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "PARSER ERROR: " << e.what() << "\n";
        return 1;
    }

    return 0;
}