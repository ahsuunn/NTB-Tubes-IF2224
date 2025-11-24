#ifndef AST_BUILDER_HPP
#define AST_BUILDER_HPP

#include "ast.hpp"
#include "parse_tree_nodes.hpp"
#include <memory>

// ast builder - mengkonversi parse tree menjadi abstract syntax tree
// menggunakan syntax-directed translation scheme
class ASTBuilder {
public:
    // main entry point - build AST from parse tree
    std::unique_ptr<ASTProgramNode> buildAST(const ProgramNode* parse_tree);
    
private:
    // translation functions untuk setiap production rule
    
    // program → program-header declaration-part compound-statement
    std::unique_ptr<ASTProgramNode> translateProgram(const ProgramNode* node);
    
    // declaration-part → var-declaration*
    std::unique_ptr<ASTDeclarationListNode> translateDeclarations(const DeclarationPartNode* node);
    
    // var-declaration → KEYWORD(variabel) identifier-list : type ;
    std::unique_ptr<ASTVarDeclNode> translateVarDeclaration(const VariableDeclarationNode* node);
    
    // compound-statement → KEYWORD(mulai) statement-list KEYWORD(selesai)
    std::unique_ptr<ASTCompoundStmtNode> translateCompoundStatement(const CompoundStatementNode* node);
    
    // statement → assignment | if | while | for | procedure-call | compound
    std::unique_ptr<ASTStatementNode> translateStatement(const ParseTreeNode* node);
    
    // assignment-statement → identifier := expression
    std::unique_ptr<ASTAssignmentNode> translateAssignment(const AssignmentStatementNode* node);
    
    // if-statement → jika expression maka statement (selain-itu statement)?
    std::unique_ptr<ASTIfNode> translateIf(const IfStatementNode* node);
    
    // while-statement → selama expression lakukan statement
    std::unique_ptr<ASTWhileNode> translateWhile(const WhileStatementNode* node);
    
    // for-statement → untuk identifier := expr ke/turun-ke expr lakukan statement
    std::unique_ptr<ASTForNode> translateFor(const ForStatementNode* node);
    
    // procedure-call → identifier (expression-list)?
    std::unique_ptr<ASTProcedureCallNode> translateProcedureCall(const ProcedureFunctionCallNode* node);
    
    // expression → simple-expression (relational-op simple-expression)?
    std::unique_ptr<ASTExpressionNode> translateExpression(const ParseTreeNode* node);
    
    // simple-expression → term (additive-op term)*
    std::unique_ptr<ASTExpressionNode> translateSimpleExpression(const SimpleExpressionNode* node);
    
    // term → factor (multiplicative-op factor)*
    std::unique_ptr<ASTExpressionNode> translateTerm(const TermNode* node);
    
    // factor → identifier | number | string | ( expression ) | not factor | function-call
    std::unique_ptr<ASTExpressionNode> translateFactor(const FactorNode* node);
    
    // helper - extract identifier names from identifier list
    std::vector<std::string> extractIdentifiers(const IdentifierListNode* node);
    
    // helper - determine literal type from token
    std::string getLiteralType(const Token& token);
};

#endif // AST_BUILDER_HPP
