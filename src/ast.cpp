#include "ast.hpp"
#include "ast_visitor.hpp"

void ASTProgramNode::accept(ASTVisitor* visitor) {
    visitor->visitProgram(this);
}

void ASTDeclarationListNode::accept(ASTVisitor* visitor) {
    visitor->visitDeclarationList(this);
}

void ASTVarDeclNode::accept(ASTVisitor* visitor) {
    visitor->visitVarDecl(this);
}

void ASTCompoundStmtNode::accept(ASTVisitor* visitor) {
    visitor->visitCompoundStmt(this);
}

void ASTAssignmentNode::accept(ASTVisitor* visitor) {
    visitor->visitAssignment(this);
}

void ASTIfNode::accept(ASTVisitor* visitor) {
    visitor->visitIf(this);
}

void ASTWhileNode::accept(ASTVisitor* visitor) {
    visitor->visitWhile(this);
}

void ASTForNode::accept(ASTVisitor* visitor) {
    visitor->visitFor(this);
}

void ASTProcedureCallNode::accept(ASTVisitor* visitor) {
    visitor->visitProcedureCall(this);
}

void ASTBinaryOpNode::accept(ASTVisitor* visitor) {
    visitor->visitBinaryOp(this);
}

void ASTUnaryOpNode::accept(ASTVisitor* visitor) {
    visitor->visitUnaryOp(this);
}

void ASTLiteralNode::accept(ASTVisitor* visitor) {
    visitor->visitLiteral(this);
}

void ASTIdentifierNode::accept(ASTVisitor* visitor) {
    visitor->visitIdentifier(this);
}

void ASTFunctionCallNode::accept(ASTVisitor* visitor) {
    visitor->visitFunctionCall(this);
}
