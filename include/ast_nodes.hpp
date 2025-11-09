#ifndef AST_NODES_HPP
#define AST_NODES_HPP

#include "token.hpp"
#include <vector>
#include <memory>
#include <string>

// Base AST Node
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual std::string toString() const { return "ASTNode"; }
    virtual std::vector<ASTNode*> getChildren() const { return {}; }
};

// Program Node
class ProgramNode : public ASTNode {
public:
    std::string pars_program_name;
    std::unique_ptr<ASTNode> pars_program_header;
    std::unique_ptr<class DeclarationPartNode> pars_declaration_part;
    std::unique_ptr<class CompoundStatementNode> pars_compound_statement;
    Token dot_token;  // DOT token at end
    
    std::string toString() const override { return "<program>"; }
    std::vector<ASTNode*> getChildren() const override;
};

// Declaration Part Node
class DeclarationPartNode : public ASTNode {
public:
    std::vector<std::unique_ptr<class VariableDeclarationNode>> pars_variable_declaration_list;
    
    std::string toString() const override { return "<declaration-part>"; }
    std::vector<ASTNode*> getChildren() const override;
};

// Program Header Node - untuk menyimpan token-token dari header
class ProgramHeaderNode : public ASTNode {
public:
    Token program_keyword;  // KEYWORD(program)
    Token program_name;     // IDENTIFIER(name)
    Token semicolon;        // SEMICOLON(;)
    
    std::string toString() const override { return "<program-header>"; }
    std::vector<ASTNode*> getChildren() const override { return {}; }
};

// Variable Declaration Node
class VariableDeclarationNode : public ASTNode {
public:
    Token var_keyword;  // KEYWORD(variabel)
    std::unique_ptr<class IdentifierListNode> pars_identifier_list;
    Token colon;        // COLON(:)
    std::unique_ptr<class TypeNode> pars_type;
    Token semicolon;    // SEMICOLON(;)
    
    std::string toString() const override { return "<var-declaration>"; }
    std::vector<ASTNode*> getChildren() const override;
};

// Identifier List Node
class IdentifierListNode : public ASTNode {
public:
    std::vector<std::string> pars_identifier_list;
    std::vector<Token> identifier_tokens;  // Store actual tokens
    std::vector<Token> comma_tokens;       // Store comma tokens
    
    std::string toString() const override { return "<identifier-list>"; }
    std::vector<ASTNode*> getChildren() const override;
};

// Type Node
class TypeNode : public ASTNode {
public:
    std::string pars_type_name;
    Token type_keyword; 
    
    std::string toString() const override { return "<type>"; }
    std::vector<ASTNode*> getChildren() const override;
};

// Compound Statement Node
class CompoundStatementNode : public ASTNode {
public:
    Token mulai_keyword;   // KEYWORD(mulai)
    std::vector<std::unique_ptr<ASTNode>> pars_statement_list;
    Token selesai_keyword; // KEYWORD(selesai)
    
    std::string toString() const override { return "<compound-statement>"; }
    std::vector<ASTNode*> getChildren() const override;
};

// Statement List Node
class StatementListNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> pars_statements;
    
    std::string toString() const override { return "<statement-list>"; }
    std::vector<ASTNode*> getChildren() const override;
};

// Token Node (for terminal symbols)
class TokenNode : public ASTNode {
public:
    Token token;
    
    TokenNode(const Token& t) : token(t) {}
    
    std::string toString() const override {
        return token.toString();
    }
    std::vector<ASTNode*> getChildren() const override { return {}; }
};

class AssignmentStatementNode : public ASTNode {
public:
    Token identifier;
    Token assign_operator;
    std::unique_ptr<ASTNode> pars_expression;
    
    std::string toString() const override { return "<assignment-statement>"; }
    std::vector<ASTNode*> getChildren() const override;
};

class IfStatementNode : public ASTNode {
public:
    Token if_keyword;
    std::unique_ptr<ASTNode> pars_condition;
    Token then_keyword;
    std::unique_ptr<ASTNode> pars_then_statement;
    Token else_keyword;
    std::unique_ptr<ASTNode> pars_else_statement;
    
    std::string toString() const override { return "<if-statement>"; }
    std::vector<ASTNode*> getChildren() const override;
};

class WhileStatementNode : public ASTNode {
public:
    Token while_keyword;
    std::unique_ptr<ASTNode> pars_condition;
    Token do_keyword;
    std::unique_ptr<ASTNode> pars_body;
    
    std::string toString() const override { return "<while-statement>"; }
    std::vector<ASTNode*> getChildren() const override;
};

class ForStatementNode : public ASTNode {
public:
    Token for_keyword;
    Token control_variable;
    Token assign_operator;
    std::unique_ptr<ASTNode> pars_initial_value;
    Token direction_keyword;
    std::unique_ptr<ASTNode> pars_final_value;
    Token do_keyword;
    std::unique_ptr<ASTNode> pars_body;
    
    std::string toString() const override { return "<for-statement>"; }
    std::vector<ASTNode*> getChildren() const override;
};

class ProcedureCallNode : public ASTNode {
public:
    Token procedure_name;
    Token lparen;
    std::unique_ptr<ASTNode> pars_parameter_list;
    Token rparen;
    
    std::string toString() const override { return "<procedure-call>"; }
    std::vector<ASTNode*> getChildren() const override;
};

class ParameterListNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> pars_parameters;
    std::vector<Token> comma_tokens;
    
    std::string toString() const override { return "<parameter-list>"; }
    std::vector<ASTNode*> getChildren() const override;
};

class ExpressionNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> pars_left;
    Token relational_op;
    std::unique_ptr<ASTNode> pars_right;
    
    std::string toString() const override { return "<expression>"; }
    std::vector<ASTNode*> getChildren() const override;
};

class SimpleExpressionNode : public ASTNode {
public:
    Token sign;
    std::vector<std::unique_ptr<ASTNode>> pars_terms;
    std::vector<Token> operators;
    
    std::string toString() const override { return "<simple-expression>"; }
    std::vector<ASTNode*> getChildren() const override;
};

class TermNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> pars_factors;
    std::vector<Token> operators;
    
    std::string toString() const override { return "<term>"; }
    std::vector<ASTNode*> getChildren() const override;
};

class FactorNode : public ASTNode {
public:
    Token token;
    Token not_operator;
    std::unique_ptr<ASTNode> pars_expression;
    std::unique_ptr<ASTNode> pars_function_call;
    
    std::string toString() const override { return "<factor>"; }
    std::vector<ASTNode*> getChildren() const override;
};

class FunctionCallNode : public ASTNode {
public:
    Token function_name;
    Token lparen;
    std::unique_ptr<ASTNode> pars_parameter_list;
    Token rparen;
    
    std::string toString() const override { return "<function-call>"; }
    std::vector<ASTNode*> getChildren() const override;
};

#endif // AST_NODES_HPP
