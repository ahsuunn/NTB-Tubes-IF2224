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
            std::cout << new_prefix << "├── " << "<statement-list>" << "\n";
            std::string stmt_list_prefix = new_prefix + "│   ";
            
            for (size_t i = 0; i < compound->pars_statement_list.size(); i++) {
                auto* stmt = compound->pars_statement_list[i].get();
                
                // Skip TokenNode with SEMICOLON - they're just separators
                if (auto* token_node = dynamic_cast<const TokenNode*>(stmt)) {
                    if (token_node->token.type == "SEMICOLON") {
                        std::cout << stmt_list_prefix << (i == compound->pars_statement_list.size() - 1 ? "└── " : "├── ");
                        std::cout << "SEMICOLON(;)" << "\n";
                        continue; // Skip recursive print_parse_tree call
                    }
                }
                
                bool is_last = (i == compound->pars_statement_list.size() - 1);
                print_parse_tree(stmt, stmt_list_prefix, is_last, false);
            }
        }
        
        print_token(compound->selesai_keyword.type, compound->selesai_keyword.value, new_prefix, true);
        return;
    }
    
    if (auto* assign = dynamic_cast<const AssignmentStatementNode*>(node)) {
        print_token(assign->identifier.type, assign->identifier.value, new_prefix, false);
        print_token(assign->assign_operator.type, assign->assign_operator.value, new_prefix, false);
        if (assign->pars_expression) {
            print_parse_tree(assign->pars_expression.get(), new_prefix, true, false);
        }
        return;
    }
    
    if (auto* if_stmt = dynamic_cast<const IfStatementNode*>(node)) {
        print_token(if_stmt->if_keyword.type, if_stmt->if_keyword.value, new_prefix, false);
        if (if_stmt->pars_condition) {
            print_parse_tree(if_stmt->pars_condition.get(), new_prefix, false, false);
        }
        print_token(if_stmt->then_keyword.type, if_stmt->then_keyword.value, new_prefix, false);
        if (if_stmt->pars_then_statement) {
            bool has_else = if_stmt->pars_else_statement != nullptr;
            print_parse_tree(if_stmt->pars_then_statement.get(), new_prefix, !has_else, false);
        }
        if (if_stmt->pars_else_statement) {
            print_token(if_stmt->else_keyword.type, if_stmt->else_keyword.value, new_prefix, false);
            print_parse_tree(if_stmt->pars_else_statement.get(), new_prefix, true, false);
        }
        return;
    }
    
    if (auto* while_stmt = dynamic_cast<const WhileStatementNode*>(node)) {
        print_token(while_stmt->while_keyword.type, while_stmt->while_keyword.value, new_prefix, false);
        if (while_stmt->pars_condition) {
            print_parse_tree(while_stmt->pars_condition.get(), new_prefix, false, false);
        }
        print_token(while_stmt->do_keyword.type, while_stmt->do_keyword.value, new_prefix, false);
        if (while_stmt->pars_body) {
            print_parse_tree(while_stmt->pars_body.get(), new_prefix, true, false);
        }
        return;
    }
    
    if (auto* for_node = dynamic_cast<const ForStatementNode*>(node)) {
        print_token(for_node->for_keyword.type, for_node->for_keyword.value, new_prefix, false);
        print_token(for_node->control_variable.type, for_node->control_variable.value, new_prefix, false);
        print_token(for_node->assign_operator.type, for_node->assign_operator.value, new_prefix, false);
        if (for_node->pars_initial_value) {
            print_parse_tree(for_node->pars_initial_value.get(), new_prefix, false, false);
        }
        print_token(for_node->direction_keyword.type, for_node->direction_keyword.value, new_prefix, false);
        if (for_node->pars_final_value) {
            print_parse_tree(for_node->pars_final_value.get(), new_prefix, false, false);
        }
        print_token(for_node->do_keyword.type, for_node->do_keyword.value, new_prefix, false);
        if (for_node->pars_body) {
            print_parse_tree(for_node->pars_body.get(), new_prefix, true, false);
        }
        return;
    }
    
    if (auto* proc_call = dynamic_cast<const ProcedureFunctionCallNode*>(node)) {
        print_token(proc_call->procedure_name.type, proc_call->procedure_name.value, new_prefix, false);
        if (!proc_call->lparen.value.empty()) {
            print_token(proc_call->lparen.type, proc_call->lparen.value, new_prefix, false);
        }
        if (proc_call->pars_parameter_list) {
            print_parse_tree(proc_call->pars_parameter_list.get(), new_prefix, false, false);
        }
        if (!proc_call->rparen.value.empty()) {
            print_token(proc_call->rparen.type, proc_call->rparen.value, new_prefix, true);
        }
        return;
    }
    
    if (auto* param_list = dynamic_cast<const ParameterListNode*>(node)) {
        for (size_t i = 0; i < param_list->pars_parameters.size(); i++) {
            bool is_last_param = (i == param_list->pars_parameters.size() - 1);
            print_parse_tree(param_list->pars_parameters[i].get(), new_prefix, is_last_param, false);
            if (i < param_list->comma_tokens.size()) {
                print_token(param_list->comma_tokens[i].type, 
                           param_list->comma_tokens[i].value, new_prefix, false);
            }
        }
        return;
    }
    
    if (auto* expr = dynamic_cast<const ExpressionNode*>(node)) {
        if (expr->pars_left) {
            bool has_right = expr->pars_right != nullptr;
            print_parse_tree(expr->pars_left.get(), new_prefix, !has_right, false);
        }
        if (expr->pars_relational_op) {
            print_parse_tree(expr->pars_relational_op.get(), new_prefix, false, false);
        }
        if (expr->pars_right) {
            print_parse_tree(expr->pars_right.get(), new_prefix, true, false);
        }
        return;
    }
    
    if (auto* simple_expr = dynamic_cast<const SimpleExpressionNode*>(node)) {
        if (!simple_expr->sign.value.empty()) {
            print_token(simple_expr->sign.type, simple_expr->sign.value, new_prefix, false);
        }
        for (size_t i = 0; i < simple_expr->pars_terms.size(); i++) {
            bool is_last_term = (i == simple_expr->pars_terms.size() - 1 && i >= simple_expr->pars_operators.size());
            print_parse_tree(simple_expr->pars_terms[i].get(), new_prefix, is_last_term, false);
            if (i < simple_expr->pars_operators.size()) {
                print_parse_tree(simple_expr->pars_operators[i].get(), new_prefix, 
                               i == simple_expr->pars_operators.size() - 1, false);
            }
        }
        return;
    }
    
    if (auto* term = dynamic_cast<const TermNode*>(node)) {
        for (size_t i = 0; i < term->pars_factors.size(); i++) {
            bool is_last_factor = (i == term->pars_factors.size() - 1 && i >= term->pars_operators.size());
            print_parse_tree(term->pars_factors[i].get(), new_prefix, is_last_factor, false);
            if (i < term->pars_operators.size()) {
                print_parse_tree(term->pars_operators[i].get(), new_prefix, 
                               i == term->pars_operators.size() - 1, false);
            }
        }
        return;
    }
    
    if (auto* factor = dynamic_cast<const FactorNode*>(node)) {
        if (!factor->not_operator.value.empty()) {
            print_token(factor->not_operator.type, factor->not_operator.value, new_prefix, false);
            if (factor->pars_expression) {
                print_parse_tree(factor->pars_expression.get(), new_prefix, true, false);
            }
            return;
        }
        
        if (factor->pars_procedure_function_call) {
            print_parse_tree(factor->pars_procedure_function_call.get(), new_prefix, true, false);
            return;
        }
        
        if (factor->pars_expression) {
            print_parse_tree(factor->pars_expression.get(), new_prefix, true, false);
            return;
        }
        
        if (!factor->token.value.empty()) {
            print_token(factor->token.type, factor->token.value, new_prefix, true);
            return;
        }
        return;
    }
    
    if (auto* token_node = dynamic_cast<const TokenNode*>(node)) {
        print_token(token_node->token.type, token_node->token.value, new_prefix, is_last);
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
