#include "parser.hpp"
#include <stdexcept>
#include <sstream>

Parser::Parser(const std::vector<Token>& tokens) 
    : tokens(tokens), current_pos(0) {
    if (!tokens.empty()) {
        current_token = tokens[0];
        while (current_token.type == "COMMENT" && current_pos < tokens.size() - 1) {
            current_pos++;
            current_token = tokens[current_pos];
        }
    }
}

void Parser::advance() {
    if (current_pos < tokens.size() - 1) {
        current_pos++;
        current_token = tokens[current_pos];
        while (current_token.type == "COMMENT" && current_pos < tokens.size() - 1) {
            current_pos++;
            current_token = tokens[current_pos];
        }
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
           << ": " << message << "\n"
           << "  Expected: " << type << "\n"
           << "  Got: " << current_token.type << "(" << current_token.value << ")";
        
        if (current_pos > 0 && current_pos < tokens.size()) {
            ss << "\n  Context: ";
            if (current_pos >= 2) ss << tokens[current_pos-2].value << " ";
            if (current_pos >= 1) ss << tokens[current_pos-1].value << " ";
            ss << ">>> " << current_token.value << " <<<";
            if (current_pos + 1 < tokens.size()) ss << " " << tokens[current_pos+1].value;
            if (current_pos + 2 < tokens.size()) ss << " " << tokens[current_pos+2].value;
        }
        
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

std::unique_ptr<ProgramNode> Parser::pars_program() {
    auto prog_node = std::make_unique<ProgramNode>();
    
    auto header = pars_program_header();
    if (auto* prog_header = dynamic_cast<ProgramHeaderNode*>(header.get())) {
        prog_node->pars_program_name = prog_header->program_name.value;
    }
    prog_node->pars_program_header = std::move(header);
    
    prog_node->pars_declaration_part = pars_declaration_part();
    
    prog_node->pars_compound_statement = pars_compound_statement();
    
    if (!check("DOT")) {
        throw SyntaxError("Expected '.' at end of program");
    }
    prog_node->dot_token = current_token;
    advance();
    
    return prog_node;
}

std::unique_ptr<ParseTreeNode> Parser::pars_program_header() {
    auto header_node = std::make_unique<ProgramHeaderNode>();
    
    if (!check("KEYWORD") || current_token.value != "program") {
        std::stringstream ss;
        ss << "Error at line " << current_token.line << ", column " << current_token.column 
           << ": Expected keyword 'program' at the beginning of the program\n"
           << "  Got: " << current_token.type << "(" << current_token.value << ")";
        throw SyntaxError(ss.str());
    }
    header_node->program_keyword = current_token; 
    advance();
    
    if (check("IDENTIFIER")) {
        header_node->program_name = current_token; 
        advance();
    } else {
        std::stringstream ss;
        ss << "Error at line " << current_token.line << ", column " << current_token.column 
           << ": Expected program name (identifier) after 'program' keyword\n"
           << "  Got: " << current_token.type << "(" << current_token.value << ")";
        throw SyntaxError(ss.str());
    }
    
    if (!check("SEMICOLON")) {
        std::stringstream ss;
        ss << "Error at line " << current_token.line << ", column " << current_token.column 
           << ": Expected ';' after program name\n"
           << "  Program name: " << header_node->program_name.value << "\n"
           << "  Got: " << current_token.type << "(" << current_token.value << ")";
        throw SyntaxError(ss.str());
    }
    header_node->semicolon = current_token;  
    advance();
    
    return header_node;
}

std::unique_ptr<DeclarationPartNode> Parser::pars_declaration_part() {
    auto decl_part_node = std::make_unique<DeclarationPartNode>();
    
    if (check("KEYWORD") && current_token.value == "konstanta") {
        Token const_keyword = current_token;
        advance(); 
        
        while (check("IDENTIFIER")) {
            auto const_decl = pars_const_declaration();
            const_decl->const_keyword = const_keyword; 
            decl_part_node->pars_const_declaration_list.push_back(std::move(const_decl));
        }
    }
    
    if (check("KEYWORD") && current_token.value == "tipe") {
        Token type_keyword = current_token;
        advance(); 
        
        while (check("IDENTIFIER")) {
            auto type_decl = pars_type_declaration();
            type_decl->type_keyword = type_keyword;
            decl_part_node->pars_type_declaration_list.push_back(std::move(type_decl));
        }
    }
    
    if (check("KEYWORD") && current_token.value == "variabel") {
        Token var_keyword = current_token;
        advance(); 
        
        while (check("IDENTIFIER")) {
            auto var_decl = pars_variable_declaration_part();
            var_decl->var_keyword = var_keyword;
            decl_part_node->pars_variable_declaration_list.push_back(std::move(var_decl));
        }
    }
    
    while (check("KEYWORD") && (current_token.value == "prosedur" || current_token.value == "fungsi")) {
        auto subprog_decl = pars_subprogram_declaration();
        decl_part_node->pars_subprogram_declaration_list.push_back(std::move(subprog_decl));
    }
    
    return decl_part_node;
}

std::unique_ptr<VariableDeclarationNode> Parser::pars_variable_declaration_part() {
    auto var_decl_node = std::make_unique<VariableDeclarationNode>();
      
    var_decl_node->pars_identifier_list = pars_identifier_list();
    
    if (!check("COLON")) {
        std::stringstream ss;
        ss << "Error at line " << current_token.line << ", column " << current_token.column 
           << ": Expected ':' after variable identifier list\n"
           << "  Variables: ";
        for (const auto& id : var_decl_node->pars_identifier_list->pars_identifier_list) {
            ss << id << " ";
        }
        ss << "\n  Got: " << current_token.type << "(" << current_token.value << ")";
        throw SyntaxError(ss.str());
    }
    var_decl_node->colon = current_token;
    advance();
    
    var_decl_node->pars_type = pars_type();
    
    if (!check("SEMICOLON")) {
        std::stringstream ss;
        ss << "Error at line " << current_token.line << ", column " << current_token.column 
           << ": Expected ';' after variable type declaration\n"
           << "  Got: " << current_token.type << "(" << current_token.value << ")";
        throw SyntaxError(ss.str());
    }
    var_decl_node->semicolon = current_token;
    advance();
    
    return var_decl_node;
}

std::unique_ptr<ConstDeclarationNode> Parser::pars_const_declaration() {
    auto const_decl_node = std::make_unique<ConstDeclarationNode>();
    
    if (!check("IDENTIFIER")) {
        throw SyntaxError("Expected identifier after 'konstanta'");
    }
    const_decl_node->identifier = current_token;
    advance();
    
    if (!check("RELATIONAL_OPERATOR") || current_token.value != "=") {
        throw SyntaxError("Expected '=' after constant identifier");
    }
    const_decl_node->equal = current_token;
    advance();
    
    if (check("NUMBER") || check("STRING_LITERAL") || check("CHAR_LITERAL") ||
        (check("KEYWORD") && (current_token.value == "benar" || current_token.value == "salah" || current_token.value == "true" || current_token.value == "false"))) {
        const_decl_node->value = current_token;
        advance();
    } else {
        throw SyntaxError("Expected constant value (number, string, char, or boolean)");
    }
    
    if (!check("SEMICOLON")) {
        throw SyntaxError("Expected ';' after constant declaration");
    }
    const_decl_node->semicolon = current_token;
    advance();
    
    return const_decl_node;
}

std::unique_ptr<TypeDeclarationNode> Parser::pars_type_declaration() {
    auto type_decl_node = std::make_unique<TypeDeclarationNode>();
    
    if (!check("IDENTIFIER")) {
        throw SyntaxError("Expected identifier after 'tipe'");
    }
    type_decl_node->identifier = current_token;
    advance();
    
    if (!check("RELATIONAL_OPERATOR") || current_token.value != "=") {
        throw SyntaxError("Expected '=' after type identifier");
    }
    type_decl_node->equal = current_token;
    advance();
    
    if (check("KEYWORD") && current_token.value == "larik") {
        type_decl_node->pars_type_definition = pars_array_type();
    } 
    else if (check("KEYWORD") && (current_token.value == "integer" || current_token.value == "real" || current_token.value == "boolean" || current_token.value == "char")) {
        type_decl_node->pars_type_definition = pars_type();
    }
    else if (check("NUMBER") || check("CHAR_LITERAL") || check("IDENTIFIER")) {
         type_decl_node->pars_type_definition = pars_range();
    }
    else {
        throw SyntaxError("Expected type definition (array, simple type, or range)");
    }
    
    if (!check("SEMICOLON")) {
        throw SyntaxError("Expected ';' after type declaration");
    }
    type_decl_node->semicolon = current_token;
    advance();
    
    return type_decl_node;
}

std::unique_ptr<ParseTreeNode> Parser::pars_array_type() {
    auto array_node = std::make_unique<ArrayTypeNode>();
    
    if (!check("KEYWORD") || current_token.value != "larik") {
        throw SyntaxError("Expected keyword 'larik' for array type");
    }
    array_node->array_keyword = current_token;
    advance();
    
    if (!check("LBRACKET")) {
        throw SyntaxError("Expected '[' after 'larik'");
    }
    array_node->lbracket = current_token;
    advance();
    
    array_node->pars_range = pars_range();
    
    if (!check("RBRACKET")) {
        throw SyntaxError("Expected ']' after array range");
    }
    array_node->rbracket = current_token;
    advance();

    if (!check("KEYWORD") || current_token.value != "dari") {
        throw SyntaxError("Expected keyword 'dari' after array range");
    }
    array_node->of_keyword = current_token;
    advance();
    
    array_node->pars_type = pars_type();
    
    return array_node;
}

std::unique_ptr<RangeNode> Parser::pars_range() {
    auto range_node = std::make_unique<RangeNode>();
    
    range_node->pars_start_expression = pars_simple_expression();
    
    if (!check("RANGE_OPERATOR")) {
        throw SyntaxError("Expected '..' in range");
    }
    range_node->range_operator = current_token;
    advance();
    
    range_node->pars_end_expression = pars_simple_expression();
    
    return range_node;
}

std::unique_ptr<SubprogramDeclarationNode> Parser::pars_subprogram_declaration() {
    auto subprog_node = std::make_unique<SubprogramDeclarationNode>();
    
    if (check("KEYWORD") && current_token.value == "prosedur") {
        subprog_node->pars_declaration = pars_procedure_declaration();
    } else if (check("KEYWORD") && current_token.value == "fungsi") {
        subprog_node->pars_declaration = pars_function_declaration();
    } else {
        throw SyntaxError("Expected 'prosedur' or 'fungsi' keyword");
    }
    
    return subprog_node;
}

std::unique_ptr<ParseTreeNode> Parser::pars_procedure_declaration() {
    auto proc_node = std::make_unique<ProcedureDeclarationNode>();
    
    if (!check("KEYWORD") || current_token.value != "prosedur") {
        throw SyntaxError("Expected keyword 'prosedur'");
    }
    proc_node->procedure_keyword = current_token;
    advance();
    
    if (!check("IDENTIFIER")) {
        throw SyntaxError("Expected identifier after 'prosedur'");
    }
    proc_node->identifier = current_token;
    advance();
    
    if (check("LPARENTHESIS")) {
        proc_node->pars_formal_parameter_list = pars_formal_parameter_list();
    }
    
    if (!check("SEMICOLON")) {
        throw SyntaxError("Expected ';' after procedure header");
    }
    proc_node->semicolon1 = current_token;
    advance();
    
    proc_node->pars_block = pars_procedure_block();
    
    if (!check("SEMICOLON")) {
        throw SyntaxError("Expected ';' after procedure block");
    }
    proc_node->semicolon2 = current_token;
    advance();
    
    return proc_node;
}

std::unique_ptr<ParseTreeNode> Parser::pars_function_declaration() {
    auto func_node = std::make_unique<FunctionDeclarationNode>();
    
    if (!check("KEYWORD") || current_token.value != "fungsi") {
        throw SyntaxError("Expected keyword 'fungsi'");
    }
    func_node->function_keyword = current_token;
    advance();
    
    if (!check("IDENTIFIER")) {
        throw SyntaxError("Expected identifier after 'fungsi'");
    }
    func_node->identifier = current_token;
    advance();
    
    if (check("LPARENTHESIS")) {
        func_node->pars_formal_parameter_list = pars_formal_parameter_list();
    }
    
    if (!check("COLON")) {
        throw SyntaxError("Expected ':' after function header");
    }
    func_node->colon = current_token;
    advance();
    
    func_node->pars_return_type = pars_type();
    
    if (!check("SEMICOLON")) {
        throw SyntaxError("Expected ';' after function header");
    }
    func_node->semicolon1 = current_token;
    advance();
    
    func_node->pars_block = pars_procedure_block();
    
    if (!check("SEMICOLON")) {
        throw SyntaxError("Expected ';' after function block");
    }
    func_node->semicolon2 = current_token;
    advance();
    
    return func_node;
}

std::unique_ptr<FormalParameterListNode> Parser::pars_formal_parameter_list() {
    auto param_list_node = std::make_unique<FormalParameterListNode>();
    
    if (!check("LPARENTHESIS")) {
        throw SyntaxError("Expected '(' for parameter list");
    }
    param_list_node->lparen = current_token;
    advance();
    
    if (check("RPARENTHESIS")) {
        param_list_node->rparen = current_token;
        advance();
        return param_list_node;
    }
    
    param_list_node->pars_parameter_groups.push_back(pars_parameter_group());
    
    while (check("SEMICOLON")) {
        param_list_node->semicolon_tokens.push_back(current_token);
        advance();
        param_list_node->pars_parameter_groups.push_back(pars_parameter_group());
    }
    
    if (!check("RPARENTHESIS")) {
        throw SyntaxError("Expected ')' after parameter list");
    }
    param_list_node->rparen = current_token;
    advance();
    
    return param_list_node;
}

std::unique_ptr<ParameterGroupNode> Parser::pars_parameter_group() {
    auto param_group_node = std::make_unique<ParameterGroupNode>();
    
    param_group_node->pars_identifier_list = pars_identifier_list();
    
    if (!check("COLON")) {
        throw SyntaxError("Expected ':' after parameter identifiers");
    }
    param_group_node->colon = current_token;
    advance();
    
    param_group_node->pars_type = pars_type();
    
    return param_group_node;
}

std::unique_ptr<ParseTreeNode> Parser::pars_procedure_block() {
    auto block_node = std::make_unique<ProgramNode>();
    
    block_node->pars_declaration_part = pars_declaration_part();
    
    block_node->pars_compound_statement = pars_compound_statement();
    
    return block_node;
}

std::unique_ptr<IdentifierListNode> Parser::pars_identifier_list() {
    auto id_list_node = std::make_unique<IdentifierListNode>();
    
    if (check("IDENTIFIER")) {
        id_list_node->pars_identifier_list.push_back(current_token.value);
        id_list_node->identifier_tokens.push_back(current_token);  
        advance();

        while (match("COMMA")) {
            Token comma_token = tokens[current_pos - 1];  
            id_list_node->comma_tokens.push_back(comma_token);
            
            if (check("IDENTIFIER")) {
                id_list_node->pars_identifier_list.push_back(current_token.value);
                id_list_node->identifier_tokens.push_back(current_token); 
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

std::unique_ptr<ParseTreeNode> Parser::pars_type() {
    
    if (check("KEYWORD") && current_token.value == "larik") {
        return pars_array_type();
    }
    
    if (check("KEYWORD")) {
        std::string type_value = current_token.value;
        if (type_value == "integer" || 
            type_value == "real" || 
            type_value == "boolean" ||
            type_value == "char") {
            
            auto type_node = std::make_unique<TypeNode>();
            type_node->pars_type_name = type_value;
            type_node->type_keyword = current_token; 
            advance();
            return type_node;
        }
    }

    if (check("IDENTIFIER")) {
        auto type_node = std::make_unique<TypeNode>();
        type_node->pars_type_name = current_token.value; 
        type_node->type_keyword = current_token; 
        advance();
        return type_node;
    }
    
    throw SyntaxError("Expected type (integer, real, boolean, char, array, or custom type identifier)");
}

std::unique_ptr<CompoundStatementNode> Parser::pars_compound_statement() {
    auto compound_node = std::make_unique<CompoundStatementNode>();
    
    if (check("KEYWORD") && current_token.value == "mulai") {
        compound_node->mulai_keyword = current_token; 
        advance();
    } else {
        std::stringstream ss;
        ss << "Error at line " << current_token.line << ", column " << current_token.column 
           << ": Expected keyword 'mulai' to begin compound statement\n"
           << "  Note: All executable code must be inside 'mulai...selesai' block\n"
           << "  Got: " << current_token.type << "(" << current_token.value << ")";
        throw SyntaxError(ss.str());
    }
    
    auto stmt_list = pars_statement_list();
    compound_node->pars_statement_list = std::move(stmt_list->pars_statements);
    
    if (check("KEYWORD") && current_token.value == "selesai") {
        compound_node->selesai_keyword = current_token;  
        advance();
    } else {
        std::stringstream ss;
        ss << "Error at line " << current_token.line << ", column " << current_token.column 
           << ": Expected keyword 'selesai' to end compound statement\n"
           << "  Note: Every 'mulai' must have a matching 'selesai'\n"
           << "  Got: " << current_token.type << "(" << current_token.value << ")";
        throw SyntaxError(ss.str());
    }
    
    return compound_node;
}

std::unique_ptr<StatementListNode> Parser::pars_statement_list() {
    auto stmt_list_node = std::make_unique<StatementListNode>();
    
    if (check("KEYWORD") && current_token.value == "selesai") {
        return stmt_list_node;
    }
    
    auto stmt = pars_statement();
    stmt_list_node->pars_statements.push_back(std::move(stmt));
    
    while (match("SEMICOLON")) {
        Token semicolon_token = previous();
        stmt_list_node->pars_statements.push_back(std::make_unique<TokenNode>(semicolon_token));
        
        if (check("KEYWORD") && current_token.value == "selesai") {
            break;
        }
        auto next_stmt = pars_statement();
        stmt_list_node->pars_statements.push_back(std::move(next_stmt));
    }
    
    return stmt_list_node;
}

std::unique_ptr<ParseTreeNode> Parser::pars_statement() {
    if (check("SEMICOLON") || 
        (check("KEYWORD") && current_token.value == "selesai")) {
        return std::make_unique<ParseTreeNode>();
    }
    
    if (check("KEYWORD") && current_token.value == "mulai") {
        return pars_compound_statement();
    }
    
    if (check("KEYWORD") && current_token.value == "jika") {
        return pars_if_statement();
    }
    
    if (check("KEYWORD") && current_token.value == "selama") {
        return pars_while_statement();
    }
    
    if (check("KEYWORD") && current_token.value == "untuk") {
        return pars_for_statement();
    }

    if (check("IDENTIFIER")) {
        Token next = peek(1);
        
        if (next.type == "ASSIGN_OPERATOR") {
            return pars_assignment_statement();
        } else if (next.type == "LPARENTHESIS" || 
                   next.type == "SEMICOLON" ||
                   (next.type == "KEYWORD" && next.value == "selesai")) {
            return pars_procedure_call();
        }
    }
    
    if (check("KEYWORD")) {
        std::string keyword = current_token.value;
        if (keyword == "writeln" || keyword == "write" || keyword == "readln" || keyword == "read") {
            return pars_procedure_call();
        }
    }
    
    std::stringstream ss;
    ss << "Syntax error at line " << current_token.line 
       << ", column " << current_token.column 
       << ": Unexpected token in statement\n"
       << "  Got: " << current_token.type << "(" << current_token.value << ")\n"
       << "  Expected one of: assignment, procedure call, if, while, for, or compound statement\n"
       << "  Valid statement starters: identifier, jika, selama, untuk, mulai, writeln, write";
    throw SyntaxError(ss.str());
}

std::unique_ptr<ParseTreeNode> Parser::pars_assignment_statement() {
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

std::unique_ptr<ParseTreeNode> Parser::pars_procedure_call() {
    auto proc_call_node = std::make_unique<ProcedureFunctionCallNode>();
    
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

std::unique_ptr<ParseTreeNode> Parser::pars_if_statement() {
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

std::unique_ptr<ParseTreeNode> Parser::pars_while_statement() {
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

std::unique_ptr<ParseTreeNode> Parser::pars_for_statement() {
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

std::unique_ptr<ParseTreeNode> Parser::pars_expression() {
    auto expr_node = std::make_unique<ExpressionNode>();
    
    expr_node->pars_left = pars_simple_expression();
    
    if (check("RELATIONAL_OPERATOR") || check("LOGICAL_OPERATOR")) {
        std::string op_val = current_token.value;
        if (op_val == "=" || op_val == "<>" || op_val == "<" || 
            op_val == "<=" || op_val == ">" || op_val == ">=") {
            auto rel_op_node = std::make_unique<RelationalOperatorNode>();
            rel_op_node->op_token = current_token;
            expr_node->pars_relational_op = std::move(rel_op_node);
            advance();
            expr_node->pars_right = pars_simple_expression();
        }
    }
    
    return expr_node;
}

std::unique_ptr<ParseTreeNode> Parser::pars_simple_expression() {
    auto simple_expr_node = std::make_unique<SimpleExpressionNode>();
    
    if (check("ARITHMETIC_OPERATOR") && (current_token.value == "+" || current_token.value == "-")) {
        simple_expr_node->sign = current_token;
        advance();
    }
    
    simple_expr_node->pars_terms.push_back(pars_term());
    
    while (check("ARITHMETIC_OPERATOR") || check("LOGICAL_OPERATOR")) {
        std::string op_val = current_token.value;
        if (op_val == "+" || op_val == "-" || op_val == "atau") {
            auto add_op_node = std::make_unique<AdditiveOperatorNode>();
            add_op_node->op_token = current_token;
            simple_expr_node->pars_operators.push_back(std::move(add_op_node));
            advance();
            simple_expr_node->pars_terms.push_back(pars_term());
        } else {
            break;
        }
    }
    
    return simple_expr_node;
}

std::unique_ptr<ParseTreeNode> Parser::pars_term() {
    auto term_node = std::make_unique<TermNode>();
    
    term_node->pars_factors.push_back(pars_factor());
    
    while (check("ARITHMETIC_OPERATOR") || check("LOGICAL_OPERATOR") || check("KEYWORD")) {
        std::string op_val = current_token.value;
        if (op_val == "*" || op_val == "/" || op_val == "bagi" || 
            op_val == "mod" || op_val == "dan") {
            auto mult_op_node = std::make_unique<MultiplicativeOperatorNode>();
            mult_op_node->op_token = current_token;
            term_node->pars_operators.push_back(std::move(mult_op_node));
            advance();
            term_node->pars_factors.push_back(pars_factor());
        } else {
            break;
        }
    }
    
    return term_node;
}

std::unique_ptr<ParseTreeNode> Parser::pars_factor() {
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
            
            auto proc_func_call_node = std::make_unique<ProcedureFunctionCallNode>();
            proc_func_call_node->procedure_name = current_token;
            advance();
            
            proc_func_call_node->lparen = current_token;
            advance();
            
            if (!check("RPARENTHESIS")) {
                proc_func_call_node->pars_parameter_list = pars_parameter_list();
            }
            
            if (!check("RPARENTHESIS")) {
                throw SyntaxError("Expected ')' after parameter list");
            }
            proc_func_call_node->rparen = current_token;
            advance();
            
            factor_node->pars_procedure_function_call = std::move(proc_func_call_node);
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

std::unique_ptr<ParseTreeNode> Parser::pars_parameter_list() {
    auto param_list_node = std::make_unique<ParameterListNode>();
    
    param_list_node->pars_parameters.push_back(pars_expression());
    
    while (check("COMMA")) {
        param_list_node->comma_tokens.push_back(current_token);
        advance();
        param_list_node->pars_parameters.push_back(pars_expression());
    }
    
    return param_list_node;
}