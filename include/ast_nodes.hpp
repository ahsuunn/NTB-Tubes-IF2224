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


// Program Header Node - untuk menyimpan token-token dari header
class ProgramHeaderNode : public ASTNode {
public:
    Token program_keyword;  // KEYWORD(program)
    Token program_name;     // IDENTIFIER(name)
    Token semicolon;        // SEMICOLON(;)
    
    std::string toString() const override { return "<program-header>"; }
    std::vector<ASTNode*> getChildren() const override { return {}; }
};

// Declaration Part Node
class DeclarationPartNode : public ASTNode {
public:
    std::vector<std::unique_ptr<class ConstDeclarationNode>> pars_const_declaration_list;
    std::vector<std::unique_ptr<class TypeDeclarationNode>> pars_type_declaration_list;
    std::vector<std::unique_ptr<class VariableDeclarationNode>> pars_variable_declaration_list;
    std::vector<std::unique_ptr<class SubprogramDeclarationNode>> pars_subprogram_declaration_list;
    
    std::string toString() const override { return "<declaration-part>"; }
    std::vector<ASTNode*> getChildren() const override;
};

// Const Declaration Node
class ConstDeclarationNode : public ASTNode {
public:
    Token const_keyword;  // KEYWORD(konstanta)
    Token identifier;     // IDENTIFIER
    Token equal;          // EQUAL(=)
    Token value;          // constant value (NUMBER, STRING, etc.)
    Token semicolon;      // SEMICOLON(;)
    
    std::string toString() const override { return "<const-declaration>"; }
    std::vector<ASTNode*> getChildren() const override { return {}; }
};

// Type Declaration Node
class TypeDeclarationNode : public ASTNode {
public:
    Token type_keyword;   // KEYWORD(tipe)
    Token identifier;     // IDENTIFIER
    Token equal;          // EQUAL(=)
    std::unique_ptr<ASTNode> pars_type_definition;  // type definition (array, range, etc.)
    Token semicolon;      // SEMICOLON(;)
    
    std::string toString() const override { return "<type-declaration>"; }
    std::vector<ASTNode*> getChildren() const override;
};

// Variable Declaration Node
class VariableDeclarationNode : public ASTNode {
public:
    Token var_keyword;  // KEYWORD(variabel)
    std::unique_ptr<class IdentifierListNode> pars_identifier_list;
    Token colon;        // COLON(:)
    // FIX: Mengganti TypeNode ke ASTNode untuk mendukung tipe array anonim
    std::unique_ptr<ASTNode> pars_type; 
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


// Array Type Node
class ArrayTypeNode : public ASTNode {
public:
    Token array_keyword;  // KEYWORD(larik)
    Token lbracket;       // LBRACKET([)
    std::unique_ptr<class RangeNode> pars_range;
    Token rbracket;       // RBRACKET(])
    Token of_keyword;     // KEYWORD(dari)
    // FIX: Mengganti TypeNode ke ASTNode untuk mendukung tipe array anonim
    std::unique_ptr<ASTNode> pars_type;
    
    std::string toString() const override { return "<array-type>"; }
    std::vector<ASTNode*> getChildren() const override;
};

// Range Node
class RangeNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> pars_start_expression;
    Token range_operator;  // RANGE_OPERATOR(..)
    std::unique_ptr<ASTNode> pars_end_expression;
    
    std::string toString() const override { return "<range>"; }
    std::vector<ASTNode*> getChildren() const override;
};

// Subprogram Declaration Node
class SubprogramDeclarationNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> pars_declaration;  // ProcedureDeclarationNode or FunctionDeclarationNode
    
    std::string toString() const override { return "<subprogram-declaration>"; }
    std::vector<ASTNode*> getChildren() const override;
};

// Procedure Declaration Node
class ProcedureDeclarationNode : public ASTNode {
public:
    Token procedure_keyword;  // KEYWORD(prosedur)
    Token identifier;         // IDENTIFIER
    std::unique_ptr<class FormalParameterListNode> pars_formal_parameter_list;
    Token semicolon1;         // SEMICOLON(;)
    std::unique_ptr<ASTNode> pars_block;  // block (declarations + compound statement)
    Token semicolon2;         // SEMICOLON(;)
    
    std::string toString() const override { return "<procedure-declaration>"; }
    std::vector<ASTNode*> getChildren() const override;
};

// Function Declaration Node
class FunctionDeclarationNode : public ASTNode {
public:
    Token function_keyword;   // KEYWORD(fungsi)
    Token identifier;         // IDENTIFIER
    std::unique_ptr<class FormalParameterListNode> pars_formal_parameter_list;
    Token colon;              // COLON(:)
    // FIX: Mengganti TypeNode ke ASTNode untuk mendukung tipe array anonim
    std::unique_ptr<ASTNode> pars_return_type;
    Token semicolon1;         // SEMICOLON(;)
    std::unique_ptr<ASTNode> pars_block;  // block
    Token semicolon2;         // SEMICOLON(;)
    
    std::string toString() const override { return "<function-declaration>"; }
    std::vector<ASTNode*> getChildren() const override;
};

// Formal Parameter List Node
class FormalParameterListNode : public ASTNode {
public:
    Token lparen;  // LPARENTHESIS(()
    std::vector<std::unique_ptr<class ParameterGroupNode>> pars_parameter_groups;
    std::vector<Token> semicolon_tokens;  // SEMICOLON tokens between groups
    Token rparen;  // RPARENTHESIS())
    
    std::string toString() const override { return "<formal-parameter-list>"; }
    std::vector<ASTNode*> getChildren() const override;
};

// Parameter Group Node (for formal parameters like "x, y: integer")
class ParameterGroupNode : public ASTNode {
public:
    std::unique_ptr<class IdentifierListNode> pars_identifier_list;
    Token colon;  // COLON(:)
    // FIX: Mengganti TypeNode ke ASTNode untuk mendukung tipe array anonim
    std::unique_ptr<ASTNode> pars_type;
    
    std::string toString() const override { return "<parameter-group>"; }
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

class ProcedureFunctionCallNode : public ASTNode {
public:
    Token procedure_name;
    Token lparen;
    std::unique_ptr<ASTNode> pars_parameter_list;
    Token rparen;
    
    std::string toString() const override { return "<procedure/function-call>"; }
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
    std::unique_ptr<class RelationalOperatorNode> pars_relational_op;
    std::unique_ptr<ASTNode> pars_right;
    
    std::string toString() const override { return "<expression>"; }
    std::vector<ASTNode*> getChildren() const override;
};

class SimpleExpressionNode : public ASTNode {
public:
    Token sign;
    std::vector<std::unique_ptr<ASTNode>> pars_terms;
    std::vector<std::unique_ptr<class AdditiveOperatorNode>> pars_operators;
    
    std::string toString() const override { return "<simple-expression>"; }
    std::vector<ASTNode*> getChildren() const override;
};

class TermNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> pars_factors;
    std::vector<std::unique_ptr<class MultiplicativeOperatorNode>> pars_operators;
    
    std::string toString() const override { return "<term>"; }
    std::vector<ASTNode*> getChildren() const override;
};


class FactorNode : public ASTNode {
public:
    Token token;
    Token not_operator;
    std::unique_ptr<ASTNode> pars_expression;
    std::unique_ptr<class ProcedureFunctionCallNode> pars_procedure_function_call;
    
    std::string toString() const override { return "<factor>"; }
    std::vector<ASTNode*> getChildren() const override;
};

// Relational Operator Node
class RelationalOperatorNode : public ASTNode {
public:
    Token op_token;  // =, <>, <, <=, >, >=
    
    std::string toString() const override { return "<relational-operator>"; }
    std::vector<ASTNode*> getChildren() const override { return {}; }
};

// Additive Operator Node
class AdditiveOperatorNode : public ASTNode {
public:
    Token op_token;  // +, -, atau
    
    std::string toString() const override { return "<additive-operator>"; }
    std::vector<ASTNode*> getChildren() const override { return {}; }
};



// Multiplicative Operator Node
class MultiplicativeOperatorNode : public ASTNode {
public:
    Token op_token;  // *, /, bagi, mod, dan
    
    std::string toString() const override { return "<multiplicative-operator>"; }
    std::vector<ASTNode*> getChildren() const override { return {}; }
};

#endif // AST_NODES_HPP