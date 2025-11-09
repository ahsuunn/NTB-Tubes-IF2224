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

std::vector<ASTNode*> AssignmentStatementNode::getChildren() const {
    std::vector<ASTNode*> children;
    if (pars_expression) {
        children.push_back(pars_expression.get());
    }
    return children;
}

std::vector<ASTNode*> IfStatementNode::getChildren() const {
    std::vector<ASTNode*> children;
    if (pars_condition) {
        children.push_back(pars_condition.get());
    }
    if (pars_then_statement) {
        children.push_back(pars_then_statement.get());
    }
    if (pars_else_statement) {
        children.push_back(pars_else_statement.get());
    }
    return children;
}

std::vector<ASTNode*> WhileStatementNode::getChildren() const {
    std::vector<ASTNode*> children;
    if (pars_condition) {
        children.push_back(pars_condition.get());
    }
    if (pars_body) {
        children.push_back(pars_body.get());
    }
    return children;
}

std::vector<ASTNode*> ForStatementNode::getChildren() const {
    std::vector<ASTNode*> children;
    if (pars_initial_value) {
        children.push_back(pars_initial_value.get());
    }
    if (pars_final_value) {
        children.push_back(pars_final_value.get());
    }
    if (pars_body) {
        children.push_back(pars_body.get());
    }
    return children;
}

std::vector<ASTNode*> ProcedureCallNode::getChildren() const {
    std::vector<ASTNode*> children;
    if (pars_parameter_list) {
        children.push_back(pars_parameter_list.get());
    }
    return children;
}

std::vector<ASTNode*> ParameterListNode::getChildren() const {
    std::vector<ASTNode*> children;
    for (const auto& param : pars_parameters) {
        if (param) {
            children.push_back(param.get());
        }
    }
    return children;
}

std::vector<ASTNode*> ExpressionNode::getChildren() const {
    std::vector<ASTNode*> children;
    if (pars_left) {
        children.push_back(pars_left.get());
    }
    if (pars_right) {
        children.push_back(pars_right.get());
    }
    return children;
}

std::vector<ASTNode*> SimpleExpressionNode::getChildren() const {
    std::vector<ASTNode*> children;
    for (const auto& term : pars_terms) {
        if (term) {
            children.push_back(term.get());
        }
    }
    return children;
}

std::vector<ASTNode*> TermNode::getChildren() const {
    std::vector<ASTNode*> children;
    for (const auto& factor : pars_factors) {
        if (factor) {
            children.push_back(factor.get());
        }
    }
    return children;
}

std::vector<ASTNode*> FactorNode::getChildren() const {
    std::vector<ASTNode*> children;
    if (pars_expression) {
        children.push_back(pars_expression.get());
    }
    if (pars_function_call) {
        children.push_back(pars_function_call.get());
    }
    return children;
}

std::vector<ASTNode*> FunctionCallNode::getChildren() const {
    std::vector<ASTNode*> children;
    if (pars_parameter_list) {
        children.push_back(pars_parameter_list.get());
    }
    return children;
}
