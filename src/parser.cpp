#include "parser.hpp"
#include <stdexcept>
#include <sstream>

Parser::Parser(const std::vector<Token>& tokens) 
    : tokens(tokens), current_pos(0) {
    if (!tokens.empty()) {
        current_token = tokens[0];
    }
}

void Parser::advance() {
    if (current_pos < tokens.size() - 1) {
        current_pos++;
        current_token = tokens[current_pos];
    }
}

bool Parser::match(const std::string& type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(const std::string& type) {
    return current_token.type == type;
}

void Parser::expect(const std::string& type, const std::string& message) {
    if (!match(type)) {
        std::stringstream ss;
        ss << "Error at line " << current_token.line 
           << ", column " << current_token.column 
           << ": " << message;
        throw std::runtime_error(ss.str());
    }
}

Token Parser::peek(int offset) {
    size_t pos = current_pos + offset;
    if (pos < tokens.size()) {
        return tokens[pos];
    }
    return current_token;
}

// Grammar: program → PROGRAM identifier ; declaration_part compound_statement .
std::unique_ptr<ProgramNode> Parser::pars_program() {
    auto prog_node = std::make_unique<ProgramNode>();
    
    // Parse program header and get program name
    auto header = pars_program_header();
    if (auto* prog_header = dynamic_cast<ProgramNode*>(header.get())) {
        prog_node->pars_program_name = prog_header->pars_program_name;
    }
    prog_node->pars_program_header = std::move(header);
    
    // Parse declaration part
    prog_node->pars_declaration_part = pars_declaration_part();
    
    // Parse compound statement
    prog_node->pars_compound_statement = pars_compound_statement();
    
    // Expect DOT at end
    expect("DOT", "Expected '.' at end of program");
    
    return prog_node;
}

// Grammar: program_header → PROGRAM identifier ;
std::unique_ptr<ASTNode> Parser::pars_program_header() {
    auto header_node = std::make_unique<ProgramNode>();
    
    expect("KEYWORD", "Expected 'program'");
    
    if (check("IDENTIFIER")) {
        header_node->pars_program_name = current_token.value;
        advance();
    } else {
        throw std::runtime_error("Expected program name");
    }
    
    expect("SEMICOLON", "Expected ';' after program name");
    
    return header_node;
}

// Grammar: declaration_part → variable_declaration_part
std::unique_ptr<DeclarationPartNode> Parser::pars_declaration_part() {
    auto decl_part_node = std::make_unique<DeclarationPartNode>();
    
    // Check for VAR keyword
    if (check("KEYWORD") && current_token.value == "var") {
        advance(); // consume VAR
        
        // Parse variable declarations until BEGIN
        while (!check("KEYWORD") || current_token.value != "begin") {
            auto var_decl = pars_variable_declaration_part();
            decl_part_node->pars_variable_declaration_list.push_back(std::move(var_decl));
        }
    }
    
    return decl_part_node;
}

// Grammar: variable_declaration_part → identifier_list : type ;
std::unique_ptr<VariableDeclarationNode> Parser::pars_variable_declaration_part() {
    auto var_decl_node = std::make_unique<VariableDeclarationNode>();
    
    // Parse identifier list
    var_decl_node->pars_identifier_list = pars_identifier_list();
    
    // Expect colon
    expect("COLON", "Expected ':' after identifier list");
    
    // Parse type
    var_decl_node->pars_type = pars_type();
    
    // Expect semicolon
    expect("SEMICOLON", "Expected ';' after variable declaration");
    
    return var_decl_node;
}

// Grammar: identifier_list → IDENTIFIER | IDENTIFIER , identifier_list
std::unique_ptr<IdentifierListNode> Parser::pars_identifier_list() {
    auto id_list_node = std::make_unique<IdentifierListNode>();
    
    if (check("IDENTIFIER")) {
        id_list_node->pars_identifier_list.push_back(current_token.value);
        advance();
        
        // Parse remaining identifiers separated by comma
        while (match("COMMA")) {
            if (check("IDENTIFIER")) {
                id_list_node->pars_identifier_list.push_back(current_token.value);
                advance();
            } else {
                throw std::runtime_error("Expected identifier after ','");
            }
        }
    } else {
        throw std::runtime_error("Expected identifier");
    }
    
    return id_list_node;
}

// Grammar: type → INTEGER | REAL | BOOLEAN
std::unique_ptr<TypeNode> Parser::pars_type() {
    auto type_node = std::make_unique<TypeNode>();
    
    if (check("KEYWORD")) {
        std::string type_value = current_token.value;
        if (type_value == "integer" || 
            type_value == "real" || 
            type_value == "boolean") {
            type_node->pars_type_name = type_value;
            advance();
        } else {
            throw std::runtime_error("Expected type (integer, real, or boolean)");
        }
    } else {
        throw std::runtime_error("Expected type");
    }
    
    return type_node;
}

// Grammar: compound_statement → BEGIN statement_list END
std::unique_ptr<CompoundStatementNode> Parser::pars_compound_statement() {
    auto compound_node = std::make_unique<CompoundStatementNode>();
    
    // Expect BEGIN (dalam bahasa Indonesia: "begin")
    if (check("KEYWORD") && current_token.value == "begin") {
        advance();
    } else {
        throw std::runtime_error("Expected 'mulai'");
    }
    
    // Parse statement list
    auto stmt_list = pars_statement_list();
    compound_node->pars_statement_list = std::move(stmt_list->pars_statements);
    
    // Expect END (dalam bahasa Indonesia: "end")
    if (check("KEYWORD") && current_token.value == "end") {
        advance();
    } else {
        throw std::runtime_error("Expected 'selesai'");
    }
    
    return compound_node;
}

// Grammar: statement_list → statement | statement ; statement_list
std::unique_ptr<StatementListNode> Parser::pars_statement_list() {
    auto stmt_list_node = std::make_unique<StatementListNode>();
    
    // Check if empty (for empty begin-end blocks)
    if (check("KEYWORD") && current_token.value == "end") {
        return stmt_list_node;
    }
    
    // Parse first statement
    auto stmt = pars_statement();
    stmt_list_node->pars_statements.push_back(std::move(stmt));
    
    // Parse additional statements separated by semicolons
    while (match("SEMICOLON")) {
        // Check for END keyword (selesai)
        if (check("KEYWORD") && current_token.value == "end") {
            break; // End of statement list
        }
        auto next_stmt = pars_statement();
        stmt_list_node->pars_statements.push_back(std::move(next_stmt));
    }
    
    return stmt_list_node;
}

// Grammar: statement → assignment_statement | procedure_call | compound_statement | if_statement | while_statement | for_statement | (empty)
std::unique_ptr<ASTNode> Parser::pars_statement() {
    // Empty statement
    if (check("SEMICOLON") || 
        (check("KEYWORD") && current_token.value == "end")) {
        return std::make_unique<ASTNode>(); // Empty statement node
    }
    
    // Compound statement (nested begin-end)
    if (check("KEYWORD") && current_token.value == "begin") {
        return pars_compound_statement();
    }
    
    // If statement
    if (check("KEYWORD") && current_token.value == "jika") {
        return pars_if_statement();
    }
    
    // While statement
    if (check("KEYWORD") && current_token.value == "selama") {
        return pars_while_statement();
    }
    
    // For statement
    if (check("KEYWORD") && current_token.value == "untuk") {
        return pars_for_statement();
    }
    
    // Assignment or procedure call (both start with IDENTIFIER)
    if (check("IDENTIFIER")) {
        // Look ahead to distinguish between assignment and procedure call
        Token next = peek(1);
        
        if (next.type == "ASSIGN_OPERATOR") {
            // Assignment statement
            return pars_assignment_statement();
        } else if (next.type == "LPARENTHESIS" || 
                   next.type == "SEMICOLON" ||
                   (next.type == "KEYWORD" && next.value == "end")) {
            // Procedure call
            return pars_procedure_call();
        }
    }
    
    // Built-in procedures (writeln, write, dll)
    if (check("KEYWORD")) {
        std::string keyword = current_token.value;
        if (keyword == "writeln" || keyword == "write" || keyword == "readln" || keyword == "read") {
            return pars_procedure_call();
        }
    }
    
    std::stringstream ss;
    ss << "Unexpected token at line " << current_token.line 
       << ", column " << current_token.column 
       << ": " << current_token.value;
    throw std::runtime_error(ss.str());
}

// Placeholder functions - ini seharusnya diimplementasi oleh teman Anda yang handle bagian statement
std::unique_ptr<ASTNode> Parser::pars_assignment_statement() {
    // TODO: Implementasi
    throw std::runtime_error("pars_assignment_statement not implemented yet");
}

std::unique_ptr<ASTNode> Parser::pars_procedure_call() {
    // TODO: Implementasi
    throw std::runtime_error("pars_procedure_call not implemented yet");
}

std::unique_ptr<ASTNode> Parser::pars_if_statement() {
    // TODO: Implementasi
    throw std::runtime_error("pars_if_statement not implemented yet");
}

std::unique_ptr<ASTNode> Parser::pars_while_statement() {
    // TODO: Implementasi
    throw std::runtime_error("pars_while_statement not implemented yet");
}

std::unique_ptr<ASTNode> Parser::pars_for_statement() {
    // TODO: Implementasi
    throw std::runtime_error("pars_for_statement not implemented yet");
}