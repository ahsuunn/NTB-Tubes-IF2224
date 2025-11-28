#include "ast/ast_printer.hpp"

void ASTPrinter::printIndent() {
    for (int i = 0; i < indent_level_; i++) {
        out_ << "  ";
    }
}

void ASTPrinter::visitProgram(ASTProgramNode* node) {
    printIndent();
    out_ << "Program: " << node->program_name << "\n";
    
    increaseIndent();
    
    if (node->declarations) {
        node->declarations->accept(this);
    }
    
    if (node->main_block) {
        node->main_block->accept(this);
    }
    
    decreaseIndent();
}

void ASTPrinter::visitDeclarationList(ASTDeclarationListNode* node) {
    printIndent();
    out_ << "Declarations:\n";
    
    increaseIndent();
    for (auto& decl : node->declarations) {
        decl->accept(this);
    }
    decreaseIndent();
}

void ASTPrinter::visitVarDecl(ASTVarDeclNode* node) {
    printIndent();
    out_ << "VarDecl: ";
    
    // print identifiers
    for (size_t i = 0; i < node->identifiers.size(); i++) {
        out_ << node->identifiers[i];
        if (i < node->identifiers.size() - 1) out_ << ", ";
    }
    
    out_ << " : " << node->type_name << "\n";
}

void ASTPrinter::visitCompoundStmt(ASTCompoundStmtNode* node) {
    printIndent();
    out_ << "CompoundStmt:\n";
    
    increaseIndent();
    for (auto& stmt : node->statements) {
        stmt->accept(this);
    }
    decreaseIndent();
}

void ASTPrinter::visitAssignment(ASTAssignmentNode* node) {
    printIndent();
    out_ << "Assignment: " << node->variable_name << " := \n";
    
    increaseIndent();
    if (node->expression) {
        node->expression->accept(this);
    }
    decreaseIndent();
}

void ASTPrinter::visitIf(ASTIfNode* node) {
    printIndent();
    out_ << "If:\n";
    
    increaseIndent();
    
    printIndent();
    out_ << "Condition:\n";
    increaseIndent();
    if (node->condition) {
        node->condition->accept(this);
    }
    decreaseIndent();
    
    printIndent();
    out_ << "Then:\n";
    increaseIndent();
    if (node->then_stmt) {
        node->then_stmt->accept(this);
    }
    decreaseIndent();
    
    if (node->else_stmt) {
        printIndent();
        out_ << "Else:\n";
        increaseIndent();
        node->else_stmt->accept(this);
        decreaseIndent();
    }
    
    decreaseIndent();
}

void ASTPrinter::visitWhile(ASTWhileNode* node) {
    printIndent();
    out_ << "While:\n";
    
    increaseIndent();
    
    printIndent();
    out_ << "Condition:\n";
    increaseIndent();
    if (node->condition) {
        node->condition->accept(this);
    }
    decreaseIndent();
    
    printIndent();
    out_ << "Body:\n";
    increaseIndent();
    if (node->body) {
        node->body->accept(this);
    }
    decreaseIndent();
    
    decreaseIndent();
}

void ASTPrinter::visitFor(ASTForNode* node) {
    printIndent();
    out_ << "For: " << node->control_var << " := \n";
    
    increaseIndent();
    
    printIndent();
    out_ << "Initial:\n";
    increaseIndent();
    if (node->initial_value) {
        node->initial_value->accept(this);
    }
    decreaseIndent();
    
    printIndent();
    out_ << (node->is_downto ? "DownTo:\n" : "To:\n");
    increaseIndent();
    if (node->final_value) {
        node->final_value->accept(this);
    }
    decreaseIndent();
    
    printIndent();
    out_ << "Body:\n";
    increaseIndent();
    if (node->body) {
        node->body->accept(this);
    }
    decreaseIndent();
    
    decreaseIndent();
}

void ASTPrinter::visitProcedureCall(ASTProcedureCallNode* node) {
    printIndent();
    out_ << "ProcedureCall: " << node->procedure_name << "\n";
    
    if (!node->arguments.empty()) {
        increaseIndent();
        printIndent();
        out_ << "Arguments:\n";
        increaseIndent();
        for (auto& arg : node->arguments) {
            arg->accept(this);
        }
        decreaseIndent();
        decreaseIndent();
    }
}

void ASTPrinter::visitBinaryOp(ASTBinaryOpNode* node) {
    printIndent();
    out_ << "BinaryOp: " << node->op << "\n";
    
    increaseIndent();
    
    printIndent();
    out_ << "Left:\n";
    increaseIndent();
    if (node->left) {
        node->left->accept(this);
    }
    decreaseIndent();
    
    printIndent();
    out_ << "Right:\n";
    increaseIndent();
    if (node->right) {
        node->right->accept(this);
    }
    decreaseIndent();
    
    decreaseIndent();
}

void ASTPrinter::visitUnaryOp(ASTUnaryOpNode* node) {
    printIndent();
    out_ << "UnaryOp: " << node->op << "\n";
    
    increaseIndent();
    if (node->operand) {
        node->operand->accept(this);
    }
    decreaseIndent();
}

void ASTPrinter::visitLiteral(ASTLiteralNode* node) {
    printIndent();
    out_ << "Literal: " << node->value << " (" << node->literal_type << ")\n";
}

void ASTPrinter::visitIdentifier(ASTIdentifierNode* node) {
    printIndent();
    out_ << "Identifier: " << node->name << "\n";
}

void ASTPrinter::visitFunctionCall(ASTFunctionCallNode* node) {
    printIndent();
    out_ << "FunctionCall: " << node->function_name << "\n";
    
    if (!node->arguments.empty()) {
        increaseIndent();
        printIndent();
        out_ << "Arguments:\n";
        increaseIndent();
        for (auto& arg : node->arguments) {
            arg->accept(this);
        }
        decreaseIndent();
        decreaseIndent();
    }
}
