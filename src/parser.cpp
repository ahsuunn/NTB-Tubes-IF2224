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
        ss << "Syntax error at line " << current_token.line 
           << ", column " << current_token.column 
           << ": " << message 
           << " (got " << current_token.type << "(" << current_token.value << "))";
        throw SyntaxError(ss.str());
    }
}

Token Parser::peek(int offset) {
    size_t pos = current_pos + offset;
    if (pos < tokens.size()) {
        return tokens[pos];
    }
    return current_token;
}

Token Parser::previous() {
    if (current_pos > 0) {
        return tokens[current_pos - 1];
    }
    return current_token;
}

// Grammar: program → PROGRAM identifier ; declaration_part compound_statement .
std::unique_ptr<ProgramNode> Parser::pars_program() {
    auto prog_node = std::make_unique<ProgramNode>();
    
    // Parse program header and get program name
    auto header = pars_program_header();
    if (auto* prog_header = dynamic_cast<ProgramHeaderNode*>(header.get())) {
        prog_node->pars_program_name = prog_header->program_name.value;
    }
    prog_node->pars_program_header = std::move(header);
    
    // Parse declaration part
    prog_node->pars_declaration_part = pars_declaration_part();
    
    // Parse compound statement
    prog_node->pars_compound_statement = pars_compound_statement();
    
    // Expect DOT at end
    if (!check("DOT")) {
        throw SyntaxError("Expected '.' at end of program");
    }
    prog_node->dot_token = current_token;
    advance();
    
    return prog_node;
}

// Grammar: program_header → PROGRAM identifier ;
std::unique_ptr<ASTNode> Parser::pars_program_header() {
    auto header_node = std::make_unique<ProgramHeaderNode>();
    
    if (!check("KEYWORD") || current_token.value != "program") {
        throw SyntaxError("Expected keyword 'program'");
    }
    header_node->program_keyword = current_token;  // Save token
    advance();
    
    if (check("IDENTIFIER")) {
        header_node->program_name = current_token;  // Save token
        advance();
    } else {
        throw SyntaxError("Expected program name (identifier)");
    }
    
    if (!check("SEMICOLON")) {
        throw SyntaxError("Expected ';' after program name");
    }
    header_node->semicolon = current_token;  // Save token
    advance();
    
    return header_node;
}

// Grammar: declaration_part → variable_declaration_part
std::unique_ptr<DeclarationPartNode> Parser::pars_declaration_part() {
    auto decl_part_node = std::make_unique<DeclarationPartNode>();
    
    // Check for variabel keyword (Bahasa Indonesia untuk 'var')
    while (check("KEYWORD") && current_token.value == "variabel") {
        // Don't advance here, let pars_variable_declaration_part handle it
        auto var_decl = pars_variable_declaration_part();
        decl_part_node->pars_variable_declaration_list.push_back(std::move(var_decl));
    }
    
    return decl_part_node;
}

// Grammar: variable_declaration_part → identifier_list : type ;
std::unique_ptr<VariableDeclarationNode> Parser::pars_variable_declaration_part() {
    auto var_decl_node = std::make_unique<VariableDeclarationNode>();
    
    // Save and consume variabel keyword
    if (!check("KEYWORD") || current_token.value != "variabel") {
        throw SyntaxError("Expected keyword 'variabel'");
    }
    var_decl_node->var_keyword = current_token;
    advance();
    
    // Parse identifier list
    var_decl_node->pars_identifier_list = pars_identifier_list();
    
    // Expect colon
    if (!check("COLON")) {
        throw SyntaxError("Expected ':' after identifier list");
    }
    var_decl_node->colon = current_token;
    advance();
    
    // Parse type
    var_decl_node->pars_type = pars_type();
    
    // Expect semicolon
    if (!check("SEMICOLON")) {
        throw SyntaxError("Expected ';' after variable declaration");
    }
    var_decl_node->semicolon = current_token;
    advance();
    
    return var_decl_node;
}

// Grammar: identifier_list → IDENTIFIER | IDENTIFIER , identifier_list
std::unique_ptr<IdentifierListNode> Parser::pars_identifier_list() {
    auto id_list_node = std::make_unique<IdentifierListNode>();
    
    if (check("IDENTIFIER")) {
        id_list_node->pars_identifier_list.push_back(current_token.value);
        id_list_node->identifier_tokens.push_back(current_token);  // Save token
        advance();
        
        // Parse remaining identifiers separated by comma
        while (match("COMMA")) {
            Token comma_token = tokens[current_pos - 1];  // Get the comma token
            id_list_node->comma_tokens.push_back(comma_token);
            
            if (check("IDENTIFIER")) {
                id_list_node->pars_identifier_list.push_back(current_token.value);
                id_list_node->identifier_tokens.push_back(current_token);  // Save token
                advance();
            } else {
                throw SyntaxError("Expected identifier after ','");
            }
        }
    } else {
        throw SyntaxError("Expected identifier");
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
            type_value == "boolean" ||
            type_value == "char") {
            type_node->pars_type_name = type_value;
            type_node->type_keyword = current_token;  // Save token
            advance();
        } else {
            throw SyntaxError("Expected type (integer, real, boolean, or char)");
        }
    } else {
        throw SyntaxError("Expected type keyword");
    }
    
    return type_node;
}

// Grammar: compound_statement → BEGIN statement_list END
std::unique_ptr<CompoundStatementNode> Parser::pars_compound_statement() {
    auto compound_node = std::make_unique<CompoundStatementNode>();
    
    // Expect mulai (Bahasa Indonesia untuk BEGIN)
    if (check("KEYWORD") && current_token.value == "mulai") {
        compound_node->mulai_keyword = current_token;  // Save token
        advance();
    } else {
        throw SyntaxError("Expected keyword 'mulai'");
    }
    
    // Parse statement list
    auto stmt_list = pars_statement_list();
    compound_node->pars_statement_list = std::move(stmt_list->pars_statements);
    
    // Expect selesai (Bahasa Indonesia untuk END)
    if (check("KEYWORD") && current_token.value == "selesai") {
        compound_node->selesai_keyword = current_token;  // Save token
        advance();
    } else {
        throw SyntaxError("Expected keyword 'selesai'");
    }
    
    return compound_node;
}

// Grammar: statement_list → statement | statement ; statement_list
std::unique_ptr<StatementListNode> Parser::pars_statement_list() {
    auto stmt_list_node = std::make_unique<StatementListNode>();
    
    // Check if empty (for empty mulai-selesai blocks)
    if (check("KEYWORD") && current_token.value == "selesai") {
        return stmt_list_node;
    }
    
    // Parse first statement
    auto stmt = pars_statement();
    stmt_list_node->pars_statements.push_back(std::move(stmt));
    
    // Parse additional statements separated by semicolons
    while (match("SEMICOLON")) {
        Token semicolon_token = previous();
        stmt_list_node->pars_statements.push_back(std::make_unique<TokenNode>(semicolon_token));
        
        // Check for selesai keyword
        if (check("KEYWORD") && current_token.value == "selesai") {
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
        (check("KEYWORD") && current_token.value == "selesai")) {
        return std::make_unique<ASTNode>(); // Empty statement node
    }
    
    // Compound statement (nested mulai-selesai)
    if (check("KEYWORD") && current_token.value == "mulai") {
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
                   (next.type == "KEYWORD" && next.value == "selesai")) {
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
    ss << "Syntax error at line " << current_token.line 
       << ", column " << current_token.column 
       << ": unexpected token " << current_token.type << "(" << current_token.value << ")";
    throw SyntaxError(ss.str());
}

// Placeholder functions - ini seharusnya diimplementasi oleh teman Anda yang handle bagian statement
std::unique_ptr<ASTNode> Parser::pars_assignment_statement() {
    auto assign_node = std::make_unique<AssignmentStatementNode>();
    
    if (!check("IDENTIFIER")) {
        throw SyntaxError("Expected identifier in assignment statement");
    }
    assign_node->identifier = current_token;
    advance();
    
    if (!check("ASSIGN_OPERATOR")) {
        throw SyntaxError("Expected ':=' in assignment statement");
    }
    assign_node->assign_operator = current_token;
    advance();
    
    assign_node->pars_expression = pars_expression();
    
    return assign_node;
}

std::unique_ptr<ASTNode> Parser::pars_procedure_call() {
    auto proc_call_node = std::make_unique<ProcedureCallNode>();
    
    if (check("IDENTIFIER") || check("KEYWORD")) {
        proc_call_node->procedure_name = current_token;
        advance();
    } else {
        throw SyntaxError("Expected procedure name");
    }
    
    if (check("LPARENTHESIS")) {
        proc_call_node->lparen = current_token;
        advance();
        
        if (!check("RPARENTHESIS")) {
            proc_call_node->pars_parameter_list = pars_parameter_list();
        }
        
        if (!check("RPARENTHESIS")) {
            throw SyntaxError("Expected ')' after parameter list");
        }
        proc_call_node->rparen = current_token;
        advance();
    }
    
    return proc_call_node;
}

std::unique_ptr<ASTNode> Parser::pars_if_statement() {
    auto if_node = std::make_unique<IfStatementNode>();
    
    if (!check("KEYWORD") || current_token.value != "jika") {
        throw SyntaxError("Expected keyword 'jika'");
    }
    if_node->if_keyword = current_token;
    advance();
    
    if_node->pars_condition = pars_expression();
    
    if (!check("KEYWORD") || current_token.value != "maka") {
        throw SyntaxError("Expected keyword 'maka' after condition");
    }
    if_node->then_keyword = current_token;
    advance();
    
    if_node->pars_then_statement = pars_statement();
    
    if (check("SEMICOLON")) {
        Token next = peek(1);
        if (next.type == "KEYWORD" && next.value == "selain-itu") {
            advance();
        }
    }
    
    if (check("KEYWORD") && current_token.value == "selain-itu") {
        if_node->else_keyword = current_token;
        advance();
        if_node->pars_else_statement = pars_statement();
    }
    
    return if_node;
}

std::unique_ptr<ASTNode> Parser::pars_while_statement() {
    auto while_node = std::make_unique<WhileStatementNode>();
    
    if (!check("KEYWORD") || current_token.value != "selama") {
        throw SyntaxError("Expected keyword 'selama'");
    }
    while_node->while_keyword = current_token;
    advance();
    
    while_node->pars_condition = pars_expression();
    
    if (!check("KEYWORD") || current_token.value != "lakukan") {
        throw SyntaxError("Expected keyword 'lakukan' after condition");
    }
    while_node->do_keyword = current_token;
    advance();
    
    while_node->pars_body = pars_statement();
    
    return while_node;
}

std::unique_ptr<ASTNode> Parser::pars_for_statement() {
    auto for_node = std::make_unique<ForStatementNode>();
    
    if (!check("KEYWORD") || current_token.value != "untuk") {
        throw SyntaxError("Expected keyword 'untuk'");
    }
    for_node->for_keyword = current_token;
    advance();
    
    if (!check("IDENTIFIER")) {
        throw SyntaxError("Expected identifier after 'untuk'");
    }
    for_node->control_variable = current_token;
    advance();
    
    if (!check("ASSIGN_OPERATOR")) {
        throw SyntaxError("Expected ':=' in for statement");
    }
    for_node->assign_operator = current_token;
    advance();
    
    for_node->pars_initial_value = pars_expression();
    
    if (!check("KEYWORD") || (current_token.value != "ke" && current_token.value != "turun-ke")) {
        throw SyntaxError("Expected keyword 'ke' or 'turun-ke'");
    }
    for_node->direction_keyword = current_token;
    advance();
    
    for_node->pars_final_value = pars_expression();
    
    if (!check("KEYWORD") || current_token.value != "lakukan") {
        throw SyntaxError("Expected keyword 'lakukan' after final value");
    }
    for_node->do_keyword = current_token;
    advance();
    
    for_node->pars_body = pars_statement();
    
    return for_node;
}

std::unique_ptr<ASTNode> Parser::pars_expression() {
    auto expr_node = std::make_unique<ExpressionNode>();
    
    expr_node->pars_left = pars_simple_expression();
    
    if (check("RELATIONAL_OPERATOR") || check("LOGICAL_OPERATOR")) {
        std::string op_val = current_token.value;
        if (op_val == "=" || op_val == "<>" || op_val == "<" || 
            op_val == "<=" || op_val == ">" || op_val == ">=") {
            expr_node->relational_op = current_token;
            advance();
            expr_node->pars_right = pars_simple_expression();
        }
    }
    
    return expr_node;
}

std::unique_ptr<ASTNode> Parser::pars_simple_expression() {
    auto simple_expr_node = std::make_unique<SimpleExpressionNode>();
    
    if (check("ARITHMETIC_OPERATOR") && (current_token.value == "+" || current_token.value == "-")) {
        simple_expr_node->sign = current_token;
        advance();
    }
    
    simple_expr_node->pars_terms.push_back(pars_term());
    
    while (check("ARITHMETIC_OPERATOR") || check("LOGICAL_OPERATOR")) {
        std::string op_val = current_token.value;
        if (op_val == "+" || op_val == "-" || op_val == "atau") {
            simple_expr_node->operators.push_back(current_token);
            advance();
            simple_expr_node->pars_terms.push_back(pars_term());
        } else {
            break;
        }
    }
    
    return simple_expr_node;
}

std::unique_ptr<ASTNode> Parser::pars_term() {
    auto term_node = std::make_unique<TermNode>();
    
    term_node->pars_factors.push_back(pars_factor());
    
    while (check("ARITHMETIC_OPERATOR") || check("LOGICAL_OPERATOR") || check("KEYWORD")) {
        std::string op_val = current_token.value;
        if (op_val == "*" || op_val == "/" || op_val == "bagi" || 
            op_val == "mod" || op_val == "dan") {
            term_node->operators.push_back(current_token);
            advance();
            term_node->pars_factors.push_back(pars_factor());
        } else {
            break;
        }
    }
    
    return term_node;
}

std::unique_ptr<ASTNode> Parser::pars_factor() {
    auto factor_node = std::make_unique<FactorNode>();
    
    if (check("LOGICAL_OPERATOR") && current_token.value == "tidak") {
        factor_node->not_operator = current_token;
        advance();
        factor_node->pars_expression = pars_factor();
        return factor_node;
    }
    
    if (check("LPARENTHESIS")) {
        Token lparen = current_token;
        advance();
        factor_node->pars_expression = pars_expression();
        if (!check("RPARENTHESIS")) {
            throw SyntaxError("Expected ')' after expression");
        }
        advance();
        return factor_node;
    }
    
    if (check("NUMBER") || check("CHAR_LITERAL") || check("STRING_LITERAL")) {
        factor_node->token = current_token;
        advance();
        return factor_node;
    }
    
    if (check("IDENTIFIER")) {
        Token id_token = current_token;
        advance();
        
        if (check("LPARENTHESIS")) {
            current_pos--;
            current_token = id_token;
            
            auto func_call_node = std::make_unique<FunctionCallNode>();
            func_call_node->function_name = current_token;
            advance();
            
            func_call_node->lparen = current_token;
            advance();
            
            if (!check("RPARENTHESIS")) {
                func_call_node->pars_parameter_list = pars_parameter_list();
            }
            
            if (!check("RPARENTHESIS")) {
                throw SyntaxError("Expected ')' after parameter list");
            }
            func_call_node->rparen = current_token;
            advance();
            
            factor_node->pars_function_call = std::move(func_call_node);
            return factor_node;
        } else {
            factor_node->token = id_token;
            return factor_node;
        }
    }
    
    if (check("KEYWORD")) {
        std::string kw = current_token.value;
        if (kw == "benar" || kw == "salah" || kw == "true" || kw == "false") {
            factor_node->token = current_token;
            advance();
            return factor_node;
        }
    }
    
    std::stringstream ss;
    ss << "Syntax error at line " << current_token.line 
       << ", column " << current_token.column 
       << ": unexpected token in expression " << current_token.type << "(" << current_token.value << ")";
    throw SyntaxError(ss.str());
}

std::unique_ptr<ASTNode> Parser::pars_parameter_list() {
    auto param_list_node = std::make_unique<ParameterListNode>();
    
    param_list_node->pars_parameters.push_back(pars_expression());
    
    while (check("COMMA")) {
        param_list_node->comma_tokens.push_back(current_token);
        advance();
        param_list_node->pars_parameters.push_back(pars_expression());
    }
    
    return param_list_node;
}
