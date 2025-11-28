#include "ast/ast_builder.hpp"
#include <stdexcept>

std::unique_ptr<ASTProgramNode> ASTBuilder::buildAST(const ProgramNode* parse_tree) {
    if (!parse_tree) {
        throw std::runtime_error("parse tree is null");
    }
    return translateProgram(parse_tree);
}

std::unique_ptr<ASTProgramNode> ASTBuilder::translateProgram(const ProgramNode* node) {
    auto ast_program = std::make_unique<ASTProgramNode>();
    
    // extract program name dari parse tree
    ast_program->program_name = node->pars_program_name;
    
    // translate declaration part
    if (node->pars_declaration_part) {
        ast_program->declarations = translateDeclarations(node->pars_declaration_part.get());
    }
    
    // translate main compound statement
    if (node->pars_compound_statement) {
        ast_program->main_block = translateCompoundStatement(node->pars_compound_statement.get());
    }
    
    return ast_program;
}

std::unique_ptr<ASTDeclarationListNode> ASTBuilder::translateDeclarations(const DeclarationPartNode* node) {
    auto decl_list = std::make_unique<ASTDeclarationListNode>();
    
    // var declaration
    for (const auto& var_decl : node->pars_variable_declaration_list) {
        if (var_decl) {
            decl_list->declarations.push_back(translateVarDeclaration(var_decl.get()));
        }
    }
    
    return decl_list;
}

std::unique_ptr<ASTVarDeclNode> ASTBuilder::translateVarDeclaration(const VariableDeclarationNode* node) {
    auto var_decl = std::make_unique<ASTVarDeclNode>();
    
    if (node->pars_identifier_list) {
        var_decl->identifiers = extractIdentifiers(node->pars_identifier_list.get());
    }
    
    if (auto* type_node = dynamic_cast<const TypeNode*>(node->pars_type.get())) {
        var_decl->type_name = type_node->pars_type_name;
    }
    
    return var_decl;
}

std::unique_ptr<ASTCompoundStmtNode> ASTBuilder::translateCompoundStatement(const CompoundStatementNode* node) {
    auto compound = std::make_unique<ASTCompoundStmtNode>();
    
    for (const auto& stmt : node->pars_statement_list) {
        if (dynamic_cast<const TokenNode*>(stmt.get())) {
            continue;
        }
        
        auto ast_stmt = translateStatement(stmt.get());
        if (ast_stmt) {
            compound->statements.push_back(std::move(ast_stmt));
        }
    }
    
    return compound;
}

std::unique_ptr<ASTStatementNode> ASTBuilder::translateStatement(const ParseTreeNode* node) {
    if (!node) return nullptr;
    
    if (auto* assign = dynamic_cast<const AssignmentStatementNode*>(node)) {
        return translateAssignment(assign);
    }
    else if (auto* if_stmt = dynamic_cast<const IfStatementNode*>(node)) {
        return translateIf(if_stmt);
    }
    else if (auto* while_stmt = dynamic_cast<const WhileStatementNode*>(node)) {
        return translateWhile(while_stmt);
    }
    else if (auto* for_stmt = dynamic_cast<const ForStatementNode*>(node)) {
        return translateFor(for_stmt);
    }
    else if (auto* proc_call = dynamic_cast<const ProcedureFunctionCallNode*>(node)) {
        return translateProcedureCall(proc_call);
    }
    else if (auto* compound = dynamic_cast<const CompoundStatementNode*>(node)) {
        return translateCompoundStatement(compound);
    }
    
    return nullptr;
}

std::unique_ptr<ASTAssignmentNode> ASTBuilder::translateAssignment(const AssignmentStatementNode* node) {
    auto assignment = std::make_unique<ASTAssignmentNode>();
    
    assignment->variable_name = node->identifier.value;
    
    if (node->pars_expression) {
        assignment->expression = translateExpression(node->pars_expression.get());
    }
    
    return assignment;
}

std::unique_ptr<ASTIfNode> ASTBuilder::translateIf(const IfStatementNode* node) {
    auto if_node = std::make_unique<ASTIfNode>();
    
    if (node->pars_condition) {
        if_node->condition = translateExpression(node->pars_condition.get());
    }
    
    if (node->pars_then_statement) {
        if_node->then_stmt = translateStatement(node->pars_then_statement.get());
    }
    
    if (node->pars_else_statement) {
        if_node->else_stmt = translateStatement(node->pars_else_statement.get());
    }
    
    return if_node;
}

std::unique_ptr<ASTWhileNode> ASTBuilder::translateWhile(const WhileStatementNode* node) {
    auto while_node = std::make_unique<ASTWhileNode>();
    
    if (node->pars_condition) {
        while_node->condition = translateExpression(node->pars_condition.get());
    }
    
    if (node->pars_body) {
        while_node->body = translateStatement(node->pars_body.get());
    }
    
    return while_node;
}

std::unique_ptr<ASTForNode> ASTBuilder::translateFor(const ForStatementNode* node) {
    auto for_node = std::make_unique<ASTForNode>();
    
    for_node->control_var = node->control_variable.value;
    
    if (node->pars_initial_value) {
        for_node->initial_value = translateExpression(node->pars_initial_value.get());
    }
    
    if (node->pars_final_value) {
        for_node->final_value = translateExpression(node->pars_final_value.get());
    }
    
    for_node->is_downto = (node->direction_keyword.value == "turun-ke");
    
    if (node->pars_body) {
        for_node->body = translateStatement(node->pars_body.get());
    }
    
    return for_node;
}

std::unique_ptr<ASTProcedureCallNode> ASTBuilder::translateProcedureCall(const ProcedureFunctionCallNode* node) {
    auto proc_call = std::make_unique<ASTProcedureCallNode>();
    
    proc_call->procedure_name = node->procedure_name.value;
    
    if (node->pars_parameter_list) {
        if (auto* param_list = dynamic_cast<const ParameterListNode*>(node->pars_parameter_list.get())) {
            for (const auto& param : param_list->pars_parameters) {
                auto arg = translateExpression(param.get());
                if (arg) {
                    proc_call->arguments.push_back(std::move(arg));
                }
            }
        }
    }
    
    return proc_call;
}

std::unique_ptr<ASTExpressionNode> ASTBuilder::translateExpression(const ParseTreeNode* node) {
    if (!node) return nullptr;
    
    if (auto* expr = dynamic_cast<const ExpressionNode*>(node)) {
        if (expr->pars_relational_op && expr->pars_right) {
            auto binary_op = std::make_unique<ASTBinaryOpNode>();
            binary_op->left = translateExpression(expr->pars_left.get());
            binary_op->right = translateExpression(expr->pars_right.get());
            
            if (auto* rel_op = dynamic_cast<const RelationalOperatorNode*>(expr->pars_relational_op.get())) {
                binary_op->op = rel_op->op_token.value;
            }
            
            return binary_op;
        }
        
        return translateExpression(expr->pars_left.get());
    }
    
    if (auto* simple_expr = dynamic_cast<const SimpleExpressionNode*>(node)) {
        return translateSimpleExpression(simple_expr);
    }
    
    return nullptr;
}

std::unique_ptr<ASTExpressionNode> ASTBuilder::translateSimpleExpression(const SimpleExpressionNode* node) {
    if (node->pars_terms.empty()) return nullptr;
    
    if (node->pars_terms.size() == 1 && node->pars_operators.empty()) {
        return translateTerm(dynamic_cast<const TermNode*>(node->pars_terms[0].get()));
    }
    
    auto result = translateTerm(dynamic_cast<const TermNode*>(node->pars_terms[0].get()));
    
    for (size_t i = 0; i < node->pars_operators.size() && i + 1 < node->pars_terms.size(); i++) {
        auto binary_op = std::make_unique<ASTBinaryOpNode>();
        binary_op->left = std::move(result);
        binary_op->right = translateTerm(dynamic_cast<const TermNode*>(node->pars_terms[i + 1].get()));
        
        if (auto* add_op = dynamic_cast<const AdditiveOperatorNode*>(node->pars_operators[i].get())) {
            binary_op->op = add_op->op_token.value;
        }
        
        result = std::move(binary_op);
    }
    
    return result;
}

std::unique_ptr<ASTExpressionNode> ASTBuilder::translateTerm(const TermNode* node) {
    if (!node || node->pars_factors.empty()) return nullptr;
    
    if (node->pars_factors.size() == 1 && node->pars_operators.empty()) {
        return translateFactor(dynamic_cast<const FactorNode*>(node->pars_factors[0].get()));
    }
    
    auto result = translateFactor(dynamic_cast<const FactorNode*>(node->pars_factors[0].get()));
    
    for (size_t i = 0; i < node->pars_operators.size() && i + 1 < node->pars_factors.size(); i++) {
        auto binary_op = std::make_unique<ASTBinaryOpNode>();
        binary_op->left = std::move(result);
        binary_op->right = translateFactor(dynamic_cast<const FactorNode*>(node->pars_factors[i + 1].get()));
        
        if (auto* mul_op = dynamic_cast<const MultiplicativeOperatorNode*>(node->pars_operators[i].get())) {
            binary_op->op = mul_op->op_token.value;
        }
        
        result = std::move(binary_op);
    }
    
    return result;
}

std::unique_ptr<ASTExpressionNode> ASTBuilder::translateFactor(const FactorNode* node) {
    if (!node) return nullptr;
    
    if (!node->not_operator.value.empty()) {
        auto unary_op = std::make_unique<ASTUnaryOpNode>();
        unary_op->op = node->not_operator.value;
        if (node->pars_expression) {
            unary_op->operand = translateExpression(node->pars_expression.get());
        }
        return unary_op;
    }
    
    if (node->pars_procedure_function_call) {
        auto func_call = std::make_unique<ASTFunctionCallNode>();
        auto* call_node = dynamic_cast<const ProcedureFunctionCallNode*>(node->pars_procedure_function_call.get());
        
        func_call->function_name = call_node->procedure_name.value;
        
        if (call_node->pars_parameter_list) {
            if (auto* param_list = dynamic_cast<const ParameterListNode*>(call_node->pars_parameter_list.get())) {
                for (const auto& param : param_list->pars_parameters) {
                    auto arg = translateExpression(param.get());
                    if (arg) {
                        func_call->arguments.push_back(std::move(arg));
                    }
                }
            }
        }
        
        return func_call;
    }
    
    if (node->pars_expression) {
        return translateExpression(node->pars_expression.get());
    }
    
    if (!node->token.value.empty()) {
        if (node->token.type == "IDENTIFIER") {
            auto identifier = std::make_unique<ASTIdentifierNode>();
            identifier->name = node->token.value;
            return identifier;
        }
        else {
            auto literal = std::make_unique<ASTLiteralNode>();
            literal->value = node->token.value;
            literal->literal_type = getLiteralType(node->token);
            return literal;
        }
    }
    
    return nullptr;
}

std::vector<std::string> ASTBuilder::extractIdentifiers(const IdentifierListNode* node) {
    std::vector<std::string> identifiers;
    
    for (const auto& token : node->identifier_tokens) {
        identifiers.push_back(token.value);
    }
    
    return identifiers;
}

std::string ASTBuilder::getLiteralType(const Token& token) {
    if (token.type == "NUMBER") {
        // desimal ga
        if (token.value.find('.') != std::string::npos) {
            return "real";
        }
        return "integer";
    }
    else if (token.type == "STRING_LITERAL") {
        return "string";
    }
    else if (token.type == "CHAR_LITERAL") {
        return "char";
    }
    else if (token.type == "KEYWORD") {
        // bool benar/salah
        if (token.value == "benar" || token.value == "salah") {
            return "boolean";
        }
    }
    
    return "unknown";
}
