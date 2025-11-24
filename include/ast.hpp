#ifndef AST_HPP
#define AST_HPP

#include <string>
#include <vector>
#include <memory>
#include "token.hpp"

// base class untuk semua node AST
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual std::string getNodeType() const = 0;
    virtual void accept(class ASTVisitor* visitor) = 0;
};

// program node - root dari AST
class ASTProgramNode : public ASTNode {
public:
    std::string program_name;
    std::unique_ptr<class ASTDeclarationListNode> declarations;
    std::unique_ptr<class ASTCompoundStmtNode> main_block;
    
    std::string getNodeType() const override { return "Program"; }
    void accept(ASTVisitor* visitor) override;
};

// declaration list - kumpulan deklarasi
class ASTDeclarationListNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> declarations;
    
    std::string getNodeType() const override { return "DeclarationList"; }
    void accept(ASTVisitor* visitor) override;
};

// variable declaration - deklarasi variabel
class ASTVarDeclNode : public ASTNode {
public:
    std::vector<std::string> identifiers;
    std::string type_name;
    
    std::string getNodeType() const override { return "VarDecl"; }
    void accept(ASTVisitor* visitor) override;
};

// base class untuk semua statement
class ASTStatementNode : public ASTNode {
public:
    virtual ~ASTStatementNode() = default;
};

// compound statement - blok mulai...selesai
class ASTCompoundStmtNode : public ASTStatementNode {
public:
    std::vector<std::unique_ptr<ASTStatementNode>> statements;
    
    std::string getNodeType() const override { return "CompoundStmt"; }
    void accept(ASTVisitor* visitor) override;
};

// assignment statement - x := expr
class ASTAssignmentNode : public ASTStatementNode {
public:
    std::string variable_name;
    std::unique_ptr<class ASTExpressionNode> expression;
    
    std::string getNodeType() const override { return "Assignment"; }
    void accept(ASTVisitor* visitor) override;
};

// if statement - jika...maka...(selain-itu)?
class ASTIfNode : public ASTStatementNode {
public:
    std::unique_ptr<class ASTExpressionNode> condition;
    std::unique_ptr<ASTStatementNode> then_stmt;
    std::unique_ptr<ASTStatementNode> else_stmt;
    
    std::string getNodeType() const override { return "If"; }
    void accept(ASTVisitor* visitor) override;
};

// while statement - selama...lakukan
class ASTWhileNode : public ASTStatementNode {
public:
    std::unique_ptr<class ASTExpressionNode> condition;
    std::unique_ptr<ASTStatementNode> body;
    
    std::string getNodeType() const override { return "While"; }
    void accept(ASTVisitor* visitor) override;
};

// for statement - untuk...ke/turun-ke...lakukan
class ASTForNode : public ASTStatementNode {
public:
    std::string control_var;
    std::unique_ptr<class ASTExpressionNode> initial_value;
    std::unique_ptr<class ASTExpressionNode> final_value;
    bool is_downto;
    std::unique_ptr<ASTStatementNode> body;
    
    std::string getNodeType() const override { return "For"; }
    void accept(ASTVisitor* visitor) override;
};

// procedure call - writeln(x)
class ASTProcedureCallNode : public ASTStatementNode {
public:
    std::string procedure_name;
    std::vector<std::unique_ptr<class ASTExpressionNode>> arguments;
    
    std::string getNodeType() const override { return "ProcedureCall"; }
    void accept(ASTVisitor* visitor) override;
};

// base class untuk semua expression
class ASTExpressionNode : public ASTNode {
public:
    virtual ~ASTExpressionNode() = default;
};

// binary operation - left op right
class ASTBinaryOpNode : public ASTExpressionNode {
public:
    std::string op;
    std::unique_ptr<ASTExpressionNode> left;
    std::unique_ptr<ASTExpressionNode> right;
    
    std::string getNodeType() const override { return "BinaryOp"; }
    void accept(ASTVisitor* visitor) override;
};

// unary operation - op expr
class ASTUnaryOpNode : public ASTExpressionNode {
public:
    std::string op;
    std::unique_ptr<ASTExpressionNode> operand;
    
    std::string getNodeType() const override { return "UnaryOp"; }
    void accept(ASTVisitor* visitor) override;
};

// literal value - number, string, boolean
class ASTLiteralNode : public ASTExpressionNode {
public:
    std::string value;
    std::string literal_type; // "integer", "real", "string", "boolean", "char"
    
    std::string getNodeType() const override { return "Literal"; }
    void accept(ASTVisitor* visitor) override;
};

// identifier reference - variable name
class ASTIdentifierNode : public ASTExpressionNode {
public:
    std::string name;
    
    std::string getNodeType() const override { return "Identifier"; }
    void accept(ASTVisitor* visitor) override;
};

// function call - sqrt(16)
class ASTFunctionCallNode : public ASTExpressionNode {
public:
    std::string function_name;
    std::vector<std::unique_ptr<ASTExpressionNode>> arguments;
    
    std::string getNodeType() const override { return "FunctionCall"; }
    void accept(ASTVisitor* visitor) override;
};

#endif // AST_HPP
