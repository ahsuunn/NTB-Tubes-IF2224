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

// helper print terminal token
void print_token(const std::string& type, const std::string& value, const std::string& prefix, bool is_last) {
    std::cout << prefix;
    std::cout << (is_last ? "└── " : "├── ");
    std::cout << type << "(" << value << ")\n";
}

// helper print parse tree
void print_parse_tree(const ASTNode* node, const std::string& prefix = "", bool is_last = true, bool is_root = true) {
    if (!node) return;
    
    if (is_root) {
        std::cout << node->toString() << "\n";
    } else {
        std::cout << prefix;
        std::cout << (is_last ? "└── " : "├── ");
        std::cout << node->toString() << "\n";
    }
    
    std::string new_prefix = prefix;
    if (!is_root) {
        new_prefix += (is_last ? "    " : "│   ");
    }
    
    if (auto* prog_header = dynamic_cast<const ProgramHeaderNode*>(node)) {
        print_token(prog_header->program_keyword.type, prog_header->program_keyword.value, new_prefix, false);
        print_token(prog_header->program_name.type, prog_header->program_name.value, new_prefix, false);
        print_token(prog_header->semicolon.type, prog_header->semicolon.value, new_prefix, true);
        return;
    }
    
    if (auto* var_decl = dynamic_cast<const VariableDeclarationNode*>(node)) {
        print_token(var_decl->var_keyword.type, var_decl->var_keyword.value, new_prefix, false);
        
        if (var_decl->pars_identifier_list) {
            std::cout << new_prefix << "├── <identifier-list>\n";
            std::string id_prefix = new_prefix + "│   ";
            auto& id_list = var_decl->pars_identifier_list;
            for (size_t i = 0; i < id_list->identifier_tokens.size(); i++) {
                bool is_last_id = (i == id_list->identifier_tokens.size() - 1 && 
                                   i >= id_list->comma_tokens.size());
                print_token(id_list->identifier_tokens[i].type, 
                           id_list->identifier_tokens[i].value, 
                           id_prefix, is_last_id);
                if (i < id_list->comma_tokens.size()) {
                    print_token(id_list->comma_tokens[i].type, 
                               id_list->comma_tokens[i].value, 
                               id_prefix, false);
                }
            }
        }
        
        print_token(var_decl->colon.type, var_decl->colon.value, new_prefix, false);
        
        // print type
        if (var_decl->pars_type) {
            std::cout << new_prefix << "├── <type>\n";
            std::string type_prefix = new_prefix + "│   ";
            print_token(var_decl->pars_type->type_keyword.type, 
                       var_decl->pars_type->type_keyword.value, 
                       type_prefix, true);
        }
        
        print_token(var_decl->semicolon.type, var_decl->semicolon.value, new_prefix, true);
        return;
    }
    
    if (auto* compound = dynamic_cast<const CompoundStatementNode*>(node)) {
        print_token(compound->mulai_keyword.type, compound->mulai_keyword.value, new_prefix, false);
        
        if (!compound->pars_statement_list.empty()) {
            std::cout << new_prefix << "├── <statement-list>\n";
            std::string stmt_prefix = new_prefix + "│   ";
            for (size_t i = 0; i < compound->pars_statement_list.size(); i++) {
                print_parse_tree(compound->pars_statement_list[i].get(), stmt_prefix, 
                               i == compound->pars_statement_list.size() - 1, false);
            }
        } else {
            std::cout << new_prefix << "├── <statement-list>\n";
        }
        
        print_token(compound->selesai_keyword.type, compound->selesai_keyword.value, new_prefix, true);
        return;
    }
    
    auto children = node->getChildren();
    
    bool has_dot_token = false;
    if (is_root) {
        if (auto* prog = dynamic_cast<const ProgramNode*>(node)) {
            has_dot_token = !prog->dot_token.value.empty();
        }
    }
    
    for (size_t i = 0; i < children.size(); i++) {
        bool child_is_last = (i == children.size() - 1) && !has_dot_token;
        print_parse_tree(children[i], new_prefix, child_is_last, false);
    }
    
    if (is_root && has_dot_token) {
        if (auto* prog = dynamic_cast<const ProgramNode*>(node)) {
            std::cout << "└── " << prog->dot_token.type << "(" << prog->dot_token.value << ")\n";
        }
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
