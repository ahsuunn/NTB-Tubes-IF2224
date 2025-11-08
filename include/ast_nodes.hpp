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

// Variable Declaration Node
class VariableDeclarationNode : public ASTNode {
public:
    std::unique_ptr<class IdentifierListNode> pars_identifier_list;
    std::unique_ptr<class TypeNode> pars_type;
    
    std::string toString() const override { return "<var-declaration>"; }
    std::vector<ASTNode*> getChildren() const override;
};

// Identifier List Node
class IdentifierListNode : public ASTNode {
public:
    std::vector<std::string> pars_identifier_list;
    
    std::string toString() const override { return "<identifier-list>"; }
    std::vector<ASTNode*> getChildren() const override;
};

// Type Node
class TypeNode : public ASTNode {
public:
    std::string pars_type_name;
    
    std::string toString() const override { return "<type>"; }
    std::vector<ASTNode*> getChildren() const override;
};

// Compound Statement Node
class CompoundStatementNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> pars_statement_list;
    
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

#endif // AST_NODES_HPP
