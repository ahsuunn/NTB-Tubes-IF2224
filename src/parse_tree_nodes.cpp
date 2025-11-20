#include "parse_tree_nodes.hpp"

// ProgramNode getChildren implementation
std::vector<ParseTreeNode*> ProgramNode::getChildren() const {
    std::vector<ParseTreeNode*> children;
    
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
std::vector<ParseTreeNode*> DeclarationPartNode::getChildren() const {
    std::vector<ParseTreeNode*> children;
    
    for (const auto& const_decl : pars_const_declaration_list) {
        if (const_decl) {
            children.push_back(const_decl.get());
        }
    }
    
    for (const auto& type_decl : pars_type_declaration_list) {
        if (type_decl) {
            children.push_back(type_decl.get());
        }
    }
    
    for (const auto& var_decl : pars_variable_declaration_list) {
        if (var_decl) {
            children.push_back(var_decl.get());
        }
    }
    
    for (const auto& subprog_decl : pars_subprogram_declaration_list) {
        if (subprog_decl) {
            children.push_back(subprog_decl.get());
        }
    }
    
    return children;
}

// VariableDeclarationNode getChildren implementation
std::vector<ParseTreeNode*> VariableDeclarationNode::getChildren() const {
    std::vector<ParseTreeNode*> children;
    
    if (pars_identifier_list) {
        children.push_back(pars_identifier_list.get());
    }
    if (pars_type) {
        children.push_back(pars_type.get());
    }
    
    return children;
}

// IdentifierListNode getChildren implementation
std::vector<ParseTreeNode*> IdentifierListNode::getChildren() const {
    // Return empty - identifiers will be shown in toString()
    return {};
}

// TypeNode getChildren implementation
std::vector<ParseTreeNode*> TypeNode::getChildren() const {
    // Return empty - type name will be shown in toString()
    return {};
}

// CompoundStatementNode getChildren implementation
std::vector<ParseTreeNode*> CompoundStatementNode::getChildren() const {
    std::vector<ParseTreeNode*> children;
    
    for (const auto& stmt : pars_statement_list) {
        if (stmt) {
            children.push_back(stmt.get());
        }
    }
    
    return children;
}

// StatementListNode getChildren implementation
std::vector<ParseTreeNode*> StatementListNode::getChildren() const {
    std::vector<ParseTreeNode*> children;
    
    for (const auto& stmt : pars_statements) {
        if (stmt) {
            children.push_back(stmt.get());
        }
    }
    
    return children;
}

std::vector<ParseTreeNode*> AssignmentStatementNode::getChildren() const {
    std::vector<ParseTreeNode*> children;
    if (pars_expression) {
        children.push_back(pars_expression.get());
    }
    return children;
}

std::vector<ParseTreeNode*> IfStatementNode::getChildren() const {
    std::vector<ParseTreeNode*> children;
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

std::vector<ParseTreeNode*> WhileStatementNode::getChildren() const {
    std::vector<ParseTreeNode*> children;
    if (pars_condition) {
        children.push_back(pars_condition.get());
    }
    if (pars_body) {
        children.push_back(pars_body.get());
    }
    return children;
}

std::vector<ParseTreeNode*> ForStatementNode::getChildren() const {
    std::vector<ParseTreeNode*> children;
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

std::vector<ParseTreeNode*> ProcedureFunctionCallNode::getChildren() const {
    std::vector<ParseTreeNode*> children;
    if (pars_parameter_list) {
        children.push_back(pars_parameter_list.get());
    }
    return children;
}

std::vector<ParseTreeNode*> ParameterListNode::getChildren() const {
    std::vector<ParseTreeNode*> children;
    for (const auto& param : pars_parameters) {
        if (param) {
            children.push_back(param.get());
        }
    }
    return children;
}

std::vector<ParseTreeNode*> ExpressionNode::getChildren() const {
    std::vector<ParseTreeNode*> children;
    if (pars_left) {
        children.push_back(pars_left.get());
    }
    if (pars_relational_op) {
        children.push_back(pars_relational_op.get());
    }
    if (pars_right) {
        children.push_back(pars_right.get());
    }
    return children;
}

std::vector<ParseTreeNode*> SimpleExpressionNode::getChildren() const {
    std::vector<ParseTreeNode*> children;
    for (size_t i = 0; i < pars_terms.size(); i++) {
        if (pars_terms[i]) {
            children.push_back(pars_terms[i].get());
        }
        if (i < pars_operators.size() && pars_operators[i]) {
            children.push_back(pars_operators[i].get());
        }
    }
    return children;
}

std::vector<ParseTreeNode*> TermNode::getChildren() const {
    std::vector<ParseTreeNode*> children;
    for (size_t i = 0; i < pars_factors.size(); i++) {
        if (pars_factors[i]) {
            children.push_back(pars_factors[i].get());
        }
        if (i < pars_operators.size() && pars_operators[i]) {
            children.push_back(pars_operators[i].get());
        }
    }
    return children;
}

std::vector<ParseTreeNode*> FactorNode::getChildren() const {
    std::vector<ParseTreeNode*> children;
    if (pars_expression) {
        children.push_back(pars_expression.get());
    }
    if (pars_procedure_function_call) {
        children.push_back(pars_procedure_function_call.get());
    }
    return children;
}

// TypeDeclarationNode getChildren implementation
std::vector<ParseTreeNode*> TypeDeclarationNode::getChildren() const {
    std::vector<ParseTreeNode*> children;
    if (pars_type_definition) {
        children.push_back(pars_type_definition.get());
    }
    return children;
}

// ArrayTypeNode getChildren implementation
std::vector<ParseTreeNode*> ArrayTypeNode::getChildren() const {
    std::vector<ParseTreeNode*> children;
    if (pars_range) {
        children.push_back(pars_range.get());
    }
    if (pars_type) {
        children.push_back(pars_type.get());
    }
    return children;
}

// RangeNode getChildren implementation
std::vector<ParseTreeNode*> RangeNode::getChildren() const {
    std::vector<ParseTreeNode*> children;
    if (pars_start_expression) {
        children.push_back(pars_start_expression.get());
    }
    if (pars_end_expression) {
        children.push_back(pars_end_expression.get());
    }
    return children;
}

// SubprogramDeclarationNode getChildren implementation
std::vector<ParseTreeNode*> SubprogramDeclarationNode::getChildren() const {
    std::vector<ParseTreeNode*> children;
    if (pars_declaration) {
        children.push_back(pars_declaration.get());
    }
    return children;
}

// ProcedureDeclarationNode getChildren implementation
std::vector<ParseTreeNode*> ProcedureDeclarationNode::getChildren() const {
    std::vector<ParseTreeNode*> children;
    if (pars_formal_parameter_list) {
        children.push_back(pars_formal_parameter_list.get());
    }
    if (pars_block) {
        children.push_back(pars_block.get());
    }
    return children;
}

// FunctionDeclarationNode getChildren implementation
std::vector<ParseTreeNode*> FunctionDeclarationNode::getChildren() const {
    std::vector<ParseTreeNode*> children;
    if (pars_formal_parameter_list) {
        children.push_back(pars_formal_parameter_list.get());
    }
    if (pars_return_type) {
        children.push_back(pars_return_type.get());
    }
    if (pars_block) {
        children.push_back(pars_block.get());
    }
    return children;
}

// FormalParameterListNode getChildren implementation
std::vector<ParseTreeNode*> FormalParameterListNode::getChildren() const {
    std::vector<ParseTreeNode*> children;
    for (const auto& param_group : pars_parameter_groups) {
        if (param_group) {
            children.push_back(param_group.get());
        }
    }
    return children;
}

// ParameterGroupNode getChildren implementation
std::vector<ParseTreeNode*> ParameterGroupNode::getChildren() const {
    std::vector<ParseTreeNode*> children;
    if (pars_identifier_list) {
        children.push_back(pars_identifier_list.get());
    }
    if (pars_type) {
        children.push_back(pars_type.get());
    }
    return children;
}
