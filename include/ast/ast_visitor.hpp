#ifndef AST_VISITOR_HPP
#define AST_VISITOR_HPP

class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    virtual void visitProgram(class ASTProgramNode* node) = 0;
    virtual void visitDeclarationList(class ASTDeclarationListNode* node) = 0;
    virtual void visitVarDecl(class ASTVarDeclNode* node) = 0;
    virtual void visitCompoundStmt(class ASTCompoundStmtNode* node) = 0;
    virtual void visitAssignment(class ASTAssignmentNode* node) = 0;
    virtual void visitIf(class ASTIfNode* node) = 0;
    virtual void visitWhile(class ASTWhileNode* node) = 0;
    virtual void visitFor(class ASTForNode* node) = 0;
    virtual void visitProcedureCall(class ASTProcedureCallNode* node) = 0;
    virtual void visitBinaryOp(class ASTBinaryOpNode* node) = 0;
    virtual void visitUnaryOp(class ASTUnaryOpNode* node) = 0;
    virtual void visitLiteral(class ASTLiteralNode* node) = 0;
    virtual void visitIdentifier(class ASTIdentifierNode* node) = 0;
    virtual void visitFunctionCall(class ASTFunctionCallNode* node) = 0;
};

#endif 
