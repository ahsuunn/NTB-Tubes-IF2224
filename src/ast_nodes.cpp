#include "ast_nodes.hpp"

// ProgramNode getChildren implementation
std::vector<ASTNode*> ProgramNode::getChildren() const {
    std::vector<ASTNode*> children;
    
    if (pars_program_header) {
        children.push_back(pars_program_header.get());
    }
    if (pars_declaration_part) {
        children.push_back(pars_declaration_part.get());
    }
    if (pars_compound_statement) {
        children.push_back(pars_compound_statement.get());
    }
    
    return children;
}

// DeclarationPartNode getChildren implementation
std::vector<ASTNode*> DeclarationPartNode::getChildren() const {
    std::vector<ASTNode*> children;
    
    for (const auto& var_decl : pars_variable_declaration_list) {
        if (var_decl) {
            children.push_back(var_decl.get());
        }
    }
    
    return children;
}

// VariableDeclarationNode getChildren implementation
std::vector<ASTNode*> VariableDeclarationNode::getChildren() const {
    std::vector<ASTNode*> children;
    
    if (pars_identifier_list) {
        children.push_back(pars_identifier_list.get());
    }
    if (pars_type) {
        children.push_back(pars_type.get());
    }
    
    return children;
}

// IdentifierListNode getChildren implementation
std::vector<ASTNode*> IdentifierListNode::getChildren() const {
    // Return empty - identifiers will be shown in toString()
    return {};
}

// TypeNode getChildren implementation
std::vector<ASTNode*> TypeNode::getChildren() const {
    // Return empty - type name will be shown in toString()
    return {};
}

// CompoundStatementNode getChildren implementation
std::vector<ASTNode*> CompoundStatementNode::getChildren() const {
    std::vector<ASTNode*> children;
    
    for (const auto& stmt : pars_statement_list) {
        if (stmt) {
            children.push_back(stmt.get());
        }
    }
    
    return children;
}

// StatementListNode getChildren implementation
std::vector<ASTNode*> StatementListNode::getChildren() const {
    std::vector<ASTNode*> children;
    
    for (const auto& stmt : pars_statements) {
        if (stmt) {
            children.push_back(stmt.get());
        }
    }
    
    return children;
}
