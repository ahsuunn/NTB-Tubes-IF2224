#include "ast/ast_printer_decorated.hpp"
#include <iomanip>
#include <sstream>

void ASTDecoratedPrinter::printIndent()
{
    for (int i = 0; i < indent_level_; i++)
    {
        out_ << "|   ";
    }
}

std::string ASTDecoratedPrinter::getTypeString(BaseType type)
{
    switch (type)
    {
    case BaseType::INTS:
        return "integer";
    case BaseType::REALS:
        return "real";
    case BaseType::BOOLS:
        return "boolean";
    case BaseType::CHARS:
        return "char";
    case BaseType::ARRAYS:
        return "array";
    case BaseType::RECORDS:
        return "record";
    case BaseType::NOTYPE:
        return "notype";
    default:
        return "unknown";
    }
}

std::string ASTDecoratedPrinter::getObjectKindString(ObjectKind kind)
{
    switch (kind)
    {
    case ObjectKind::CONSTANT:
        return "constant";
    case ObjectKind::VARIABLE:
        return "variable";
    case ObjectKind::TYPE_ID:
        return "type";
    case ObjectKind::PROCEDURE:
        return "procedure";
    case ObjectKind::FUNCTION:
        return "function";
    default:
        return "unknown";
    }
}

void ASTDecoratedPrinter::visitProgram(ASTProgramNode *node)
{
    out_ << "ProgramNode(name: '" << node->program_name << "')\n";

    increaseIndent();

    // Print Declarations
    if (node->declarations)
    {
        printIndent();
        out_ << "├─ Declarations\n";
        increaseIndent();
        node->declarations->accept(this);
        decreaseIndent();
    }

    // Print Block
    if (node->main_block)
    {
        printIndent();
        out_ << "└─ Block";

        // Add block annotation
        int idx = symTab_->lookup(node->program_name);
        if (idx != -1)
        {
            TabEntry &entry = symTab_->get_tab(idx);
            out_ << " → block_index:" << entry.ref
                 << ", lev:" << entry.lev;
        }
        out_ << "\n";

        increaseIndent();
        node->main_block->accept(this);
        decreaseIndent();
    }

    decreaseIndent();
}

void ASTDecoratedPrinter::visitDeclarationList(ASTDeclarationListNode *node)
{
    for (size_t i = 0; i < node->declarations.size(); i++)
    {
        printIndent();

        if (i == node->declarations.size() - 1)
        {
            out_ << "└─ ";
        }
        else
        {
            out_ << "├─ ";
        }

        node->declarations[i]->accept(this);
    }
}

void ASTDecoratedPrinter::visitVarDecl(ASTVarDeclNode *node)
{
    // Print untuk setiap identifier
    for (size_t i = 0; i < node->identifiers.size(); i++)
    {
        if (i > 0)
        {
            printIndent();
            out_ << "├─ ";
        }

        out_ << "VarDecl('" << node->identifiers[i] << "')";

        // Add annotation
        int idx = symTab_->lookup(node->identifiers[i]);
        if (idx != -1)
        {
            TabEntry &entry = symTab_->get_tab(idx);
            out_ << " → tab_index:" << idx
                 << ", type:" << getTypeString(entry.typ)
                 << ", lev:" << entry.lev;
        }
        out_ << "\n";
    }
}

void ASTDecoratedPrinter::visitCompoundStmt(ASTCompoundStmtNode *node)
{
    for (size_t i = 0; i < node->statements.size(); i++)
    {
        printIndent();

        if (i == node->statements.size() - 1)
        {
            out_ << "└─ ";
        }
        else
        {
            out_ << "├─ ";
        }

        node->statements[i]->accept(this);
    }
}

void ASTDecoratedPrinter::visitAssignment(ASTAssignmentNode *node)
{
    out_ << "Assign('" << node->variable_name << "' := ...)";

    // Add type annotation for assignment
    int idx = symTab_->lookup(node->variable_name);
    if (idx != -1)
    {
        TabEntry &entry = symTab_->get_tab(idx);
        out_ << " → type:void";
    }
    else
    {
        out_ << " → type:void";
    }
    out_ << "\n";

    increaseIndent();

    // Print target
    printIndent();
    out_ << "├─ target '" << node->variable_name << "'";
    idx = symTab_->lookup(node->variable_name);
    if (idx != -1)
    {
        TabEntry &entry = symTab_->get_tab(idx);
        out_ << " → tab_index:" << idx
             << ", type:" << getTypeString(entry.typ);
    }
    out_ << "\n";

    // Print value
    printIndent();
    out_ << "└─ value ";
    if (node->expression)
    {
        node->expression->accept(this);
    }
    else
    {
        out_ << "\n";
    }

    decreaseIndent();
}

void ASTDecoratedPrinter::visitIf(ASTIfNode *node)
{
    out_ << "If\n";

    increaseIndent();

    // Print condition
    printIndent();
    out_ << "├─ condition\n";
    increaseIndent();
    printIndent();
    out_ << "└─ ";
    if (node->condition)
    {
        node->condition->accept(this);
    }
    decreaseIndent();

    // Print then branch
    printIndent();
    if (node->else_stmt)
    {
        out_ << "├─ then\n";
    }
    else
    {
        out_ << "└─ then\n";
    }

    increaseIndent();
    if (node->then_stmt)
    {
        node->then_stmt->accept(this);
    }
    decreaseIndent();

    // Print else branch if exists
    if (node->else_stmt)
    {
        printIndent();
        out_ << "└─ else\n";
        increaseIndent();
        node->else_stmt->accept(this);
        decreaseIndent();
    }

    decreaseIndent();
}

void ASTDecoratedPrinter::visitWhile(ASTWhileNode *node)
{
    out_ << "While\n";

    increaseIndent();

    // Print condition
    printIndent();
    out_ << "├─ condition\n";
    increaseIndent();
    printIndent();
    out_ << "└─ ";
    if (node->condition)
    {
        node->condition->accept(this);
    }
    decreaseIndent();

    // Print body
    printIndent();
    out_ << "└─ body\n";
    increaseIndent();
    if (node->body)
    {
        node->body->accept(this);
    }
    decreaseIndent();

    decreaseIndent();
}

void ASTDecoratedPrinter::visitFor(ASTForNode *node)
{
    out_ << "For('" << node->control_var << "')\n";

    increaseIndent();

    // Print start value
    printIndent();
    out_ << "├─ start\n";
    increaseIndent();
    printIndent();
    out_ << "└─ ";
    if (node->initial_value)
    {
        node->initial_value->accept(this);
    }
    decreaseIndent();

    // Print end value
    printIndent();
    out_ << "├─ end\n";
    increaseIndent();
    printIndent();
    out_ << "└─ ";
    if (node->final_value)
    {
        node->final_value->accept(this);
    }
    decreaseIndent();

    // Print body
    printIndent();
    out_ << "└─ body\n";
    increaseIndent();
    if (node->body)
    {
        node->body->accept(this);
    }
    decreaseIndent();

    decreaseIndent();
}

void ASTDecoratedPrinter::visitProcedureCall(ASTProcedureCallNode *node)
{
    out_ << node->procedure_name << "(...)";

    // Add annotation
    int idx = symTab_->lookup(node->procedure_name);
    if (idx != -1)
    {
        TabEntry &entry = symTab_->get_tab(idx);
        out_ << " → ";
        if (entry.lev == 0 && entry.obj == ObjectKind::PROCEDURE)
        {
            out_ << "predefined, ";
        }
        out_ << "tab_index:" << idx;
    }
    out_ << "\n";
}

void ASTDecoratedPrinter::visitBinaryOp(ASTBinaryOpNode *node)
{
    out_ << "BinOp '" << node->op << "'";

    // Determine result type
    BaseType result_type = BaseType::INTS;
    if (node->op == "=" || node->op == "<>" || node->op == "<" ||
        node->op == ">" || node->op == "<=" || node->op == ">=" ||
        node->op == "and" || node->op == "or")
    {
        result_type = BaseType::INTS; // Sesuai output: BinOp '+' → type:integer
    }

    out_ << " → type:" << getTypeString(result_type) << "\n";

    increaseIndent();

    // Print left operand
    printIndent();
    out_ << "├─ ";
    if (node->left)
    {
        node->left->accept(this);
    }

    // Print right operand
    printIndent();
    out_ << "└─ ";
    if (node->right)
    {
        node->right->accept(this);
    }

    decreaseIndent();
}

void ASTDecoratedPrinter::visitUnaryOp(ASTUnaryOpNode *node)
{
    out_ << "UnaryOp '" << node->op << "'";

    // Determine result type
    BaseType result_type = BaseType::INTS;
    if (node->op == "not")
    {
        result_type = BaseType::BOOLS;
    }

    out_ << " → type:" << getTypeString(result_type) << "\n";

    increaseIndent();
    printIndent();
    out_ << "└─ ";
    if (node->operand)
    {
        node->operand->accept(this);
    }
    decreaseIndent();
}

void ASTDecoratedPrinter::visitLiteral(ASTLiteralNode *node)
{
    out_ << node->value;

    // Determine type from literal_type field or value
    BaseType type = BaseType::INTS;
    if (node->literal_type == "real")
    {
        type = BaseType::REALS;
    }
    else if (node->literal_type == "boolean")
    {
        type = BaseType::BOOLS;
    }
    else if (node->literal_type == "char" || node->literal_type == "string")
    {
        type = BaseType::CHARS;
    }
    else if (node->literal_type == "integer")
    {
        type = BaseType::INTS;
    }

    out_ << " → type:" << getTypeString(type) << "\n";
}

void ASTDecoratedPrinter::visitIdentifier(ASTIdentifierNode *node)
{
    out_ << "'" << node->name << "'";

    // Add annotation
    int idx = symTab_->lookup(node->name);
    if (idx != -1)
    {
        TabEntry &entry = symTab_->get_tab(idx);
        out_ << " → tab_index:" << idx
             << ", type:" << getTypeString(entry.typ);
    }
    out_ << "\n";
}

void ASTDecoratedPrinter::visitFunctionCall(ASTFunctionCallNode *node)
{
    out_ << node->function_name << "(...)";

    // Add annotation
    int idx = symTab_->lookup(node->function_name);
    if (idx != -1)
    {
        TabEntry &entry = symTab_->get_tab(idx);
        out_ << " → tab_index:" << idx
             << ", type:" << getTypeString(entry.typ);
    }
    out_ << "\n";
}