#ifndef AST_PRINTER_HPP
#define AST_PRINTER_HPP

#include "ast.hpp"
#include "ast_visitor.hpp"
#include <iostream>
#include <string>

class ASTPrinter : public ASTVisitor {
public:
    ASTPrinter(std::ostream& out = std::cout) : out_(out), indent_level_(0) {}
    void visitProgram(ASTProgramNode* node) override;
    void visitDeclarationList(ASTDeclarationListNode* node) override;
    void visitVarDecl(ASTVarDeclNode* node) override;
    void visitCompoundStmt(ASTCompoundStmtNode* node) override;
    void visitAssignment(ASTAssignmentNode* node) override;
    void visitIf(ASTIfNode* node) override;
    void visitWhile(ASTWhileNode* node) override;
    void visitFor(ASTForNode* node) override;
    void visitProcedureCall(ASTProcedureCallNode* node) override;
    void visitBinaryOp(ASTBinaryOpNode* node) override;
    void visitUnaryOp(ASTUnaryOpNode* node) override;
    void visitLiteral(ASTLiteralNode* node) override;
    void visitIdentifier(ASTIdentifierNode* node) override;
    void visitFunctionCall(ASTFunctionCallNode* node) override;
    
private:
    std::ostream& out_;
    int indent_level_;
    
    void printIndent();
    void increaseIndent() { indent_level_++; }
    void decreaseIndent() { if (indent_level_ > 0) indent_level_--; }
};

#endif 
