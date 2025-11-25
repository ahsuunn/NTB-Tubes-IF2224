#ifndef SCOPE_TYPE_CHECKER_HPP
#define SCOPE_TYPE_CHECKER_HPP

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <stdexcept>
#include "symbol_table.hpp"
#include "parse_tree_nodes.hpp"

// Exception untuk semantic error
class SemanticError : public std::runtime_error {
public:
    explicit SemanticError(const std::string& message) 
        : std::runtime_error(message) {}
};

// Kelas untuk Scope & Type Checking (Deklarasi)
class ScopeTypeChecker {
private:
    SymbolTable* symbolTable;           // Pointer ke symbol table
    
    // Mapping tipe Pascal-S ke BaseType
    std::map<std::string, BaseType> typeMap;
    
    // Helper methods
    BaseType getBaseType(const std::string& typeStr);
    int getTypeSize(BaseType type);
    bool isDeclaredInCurrentScope(const std::string& identifier);
    int lookupIdentifier(const std::string& identifier);
    BaseType inferTypeFromValue(const std::string& value);
    int processArrayType(const ArrayTypeNode* arrayDef);

public:
    // Constructor
    explicit ScopeTypeChecker(SymbolTable* symTab);
    
    // Destructor
    ~ScopeTypeChecker() = default;
    
    // Visit methods untuk parse tree nodes
    void visitProgram(ProgramNode* node);
    void visitDeclarationPart(DeclarationPartNode* node);
    void visitVarDecl(VariableDeclarationNode* node);
    void visitConstDecl(ConstDeclarationNode* node);
    void visitTypeDecl(TypeDeclarationNode* node);
    void visitProcedureDecl(ProcedureDeclarationNode* node);
    void visitFunctionDecl(FunctionDeclarationNode* node);
    
    // Getters
    SymbolTable* getSymbolTable() const { return symbolTable; }
};

#endif // SCOPE_TYPE_CHECKER_HPP
