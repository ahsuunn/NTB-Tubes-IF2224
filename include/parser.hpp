#ifndef PARSER_HPP
#define PARSER_HPP

#include "token.hpp"
#include "parse_tree_nodes.hpp"
#include "lexer.hpp"
#include <vector>
#include <memory>
#include <string>
#include <stdexcept>

// Syntax Error Exception
class SyntaxError : public std::runtime_error {
public:
    explicit SyntaxError(const std::string& message) : std::runtime_error(message) {}
};

// Parser Class
class Parser {
private:
    std::vector<Token> tokens;
    size_t current_pos;
    Token current_token;
    
    void advance();
    bool match(const std::string& type);
    bool check(const std::string& type);
    void expect(const std::string& type, const std::string& message);
    Token peek(int offset = 1);
    Token previous();
    
public:
    Parser(const std::vector<Token>& tokens);
    
    // Main parsing function
    std::unique_ptr<ProgramNode> pars_program();
    
    // Grammar rules
    std::unique_ptr<ParseTreeNode> pars_program_header();
    std::unique_ptr<DeclarationPartNode> pars_declaration_part();
    std::unique_ptr<ConstDeclarationNode> pars_const_declaration();
    std::unique_ptr<TypeDeclarationNode> pars_type_declaration();
    std::unique_ptr<VariableDeclarationNode> pars_variable_declaration_part();
    std::unique_ptr<SubprogramDeclarationNode> pars_subprogram_declaration();
    std::unique_ptr<ParseTreeNode> pars_array_type();
    std::unique_ptr<RangeNode> pars_range();
    std::unique_ptr<ParseTreeNode> pars_procedure_declaration();
    std::unique_ptr<ParseTreeNode> pars_function_declaration();
    std::unique_ptr<FormalParameterListNode> pars_formal_parameter_list();
    std::unique_ptr<ParameterGroupNode> pars_parameter_group();
    std::unique_ptr<ParseTreeNode> pars_procedure_block();
    std::unique_ptr<IdentifierListNode> pars_identifier_list();
    std::unique_ptr<ParseTreeNode> pars_type();
    std::unique_ptr<CompoundStatementNode> pars_compound_statement();
    std::unique_ptr<StatementListNode> pars_statement_list();
    std::unique_ptr<ParseTreeNode> pars_statement();
    
    std::unique_ptr<ParseTreeNode> pars_assignment_statement();
    std::unique_ptr<ParseTreeNode> pars_procedure_call();
    std::unique_ptr<ParseTreeNode> pars_if_statement();
    std::unique_ptr<ParseTreeNode> pars_while_statement();
    std::unique_ptr<ParseTreeNode> pars_for_statement();
    
    std::unique_ptr<ParseTreeNode> pars_expression();
    std::unique_ptr<ParseTreeNode> pars_simple_expression();
    std::unique_ptr<ParseTreeNode> pars_term();
    std::unique_ptr<ParseTreeNode> pars_factor();
    std::unique_ptr<ParseTreeNode> pars_parameter_list();
};

#endif // PARSER_HPP
