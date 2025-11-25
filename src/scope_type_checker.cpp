#include "scope_type_checker.hpp"
#include <iostream>
#include <algorithm>
#include <cctype>

// Constructor
ScopeTypeChecker::ScopeTypeChecker(SymbolTable* symTab) 
    : symbolTable(symTab) {
    
    // Inisialisasi mapping tipe
    typeMap["integer"] = BaseType::INTS;
    typeMap["real"] = BaseType::REALS;
    typeMap["boolean"] = BaseType::BOOLS;
    typeMap["char"] = BaseType::CHARS;
}

// ============================================================================
// Visit Methods
// ============================================================================

void ScopeTypeChecker::visitProgram(ProgramNode* node) {
    std::cout << "[Semantic] Visiting Program: " << node->pars_program_name << std::endl;
    
    // Insert program name to symbol table
    try {
        symbolTable->insert(node->pars_program_name, ObjectKind::PROCEDURE, 
                          BaseType::NOTYPE, 0, true, 0);
        std::cout << "[Semantic] Program '" << node->pars_program_name 
                  << "' registered" << std::endl;
    } catch (const SymbolTableError& e) {
        std::cerr << "[Semantic Error] " << e.what() << std::endl;
    }
    
    // Visit declarations
    if (node->pars_declaration_part) {
        visitDeclarationPart(node->pars_declaration_part.get());
    }
    
    std::cout << "[Semantic] Program '" << node->pars_program_name 
              << "' checked successfully" << std::endl;
}

void ScopeTypeChecker::visitDeclarationPart(DeclarationPartNode* node) {
    std::cout << "[Semantic] Visiting Declaration Part" << std::endl;
    
    // Visit semua deklarasi konstanta
    for (const auto& constDecl : node->pars_const_declaration_list) {
        if (constDecl) {
            visitConstDecl(constDecl.get());
        }
    }
    
    // Visit semua deklarasi tipe
    for (const auto& typeDecl : node->pars_type_declaration_list) {
        if (typeDecl) {
            visitTypeDecl(typeDecl.get());
        }
    }
    
    // Visit semua deklarasi variabel
    for (const auto& varDecl : node->pars_variable_declaration_list) {
        if (varDecl) {
            visitVarDecl(varDecl.get());
        }
    }
    
    // Visit semua deklarasi subprogram
    for (const auto& subprogDecl : node->pars_subprogram_declaration_list) {
        if (subprogDecl) {
            // Check if it's a procedure or function declaration
            if (auto* procDecl = dynamic_cast<ProcedureDeclarationNode*>(
                    subprogDecl->pars_declaration.get())) {
                visitProcedureDecl(procDecl);
            } else if (auto* funcDecl = dynamic_cast<FunctionDeclarationNode*>(
                    subprogDecl->pars_declaration.get())) {
                visitFunctionDecl(funcDecl);
            }
        }
    }
}

void ScopeTypeChecker::visitVarDecl(VariableDeclarationNode* node) {
    // Dapatkan tipe dari node
    std::string typeStr;
    if (auto* typeNode = dynamic_cast<TypeNode*>(node->pars_type.get())) {
        typeStr = typeNode->pars_type_name;
    } else if (auto* arrayNode = dynamic_cast<ArrayTypeNode*>(node->pars_type.get())) {
        // Handle array type - akan diproses nanti
        typeStr = "array";
    } else {
        throw SemanticError("Unknown type in variable declaration");
    }
    
    BaseType varType = getBaseType(typeStr);
    
    std::cout << "[Semantic] Declaring variables: ";
    
    // Proses setiap variabel dalam identifier list
    if (node->pars_identifier_list) {
        const auto& identifiers = node->pars_identifier_list->pars_identifier_list;
        
        for (size_t i = 0; i < identifiers.size(); i++) {
            std::cout << identifiers[i];
            if (i < identifiers.size() - 1) std::cout << ", ";
            
            // Cek apakah sudah dideklarasikan di scope saat ini
            if (isDeclaredInCurrentScope(identifiers[i])) {
                throw SemanticError("Variable '" + identifiers[i] + 
                                  "' already declared in current scope");
            }
            
            // Hitung address untuk variabel
            int currentLevel = symbolTable->get_current_level();
            int blockIdx = currentLevel;
            if (blockIdx >= symbolTable->get_btab_size()) {
                blockIdx = 0;
            }
            
            int currentVsize = symbolTable->get_btab(blockIdx).vsize;
            
            // Insert ke symbol table
            try {
                int varIdx = symbolTable->insert(identifiers[i], ObjectKind::VARIABLE, 
                                                varType, 0, true, currentVsize);
                
                // Update vsize
                symbolTable->set_block_vars(blockIdx, currentVsize + getTypeSize(varType));
                
                std::cout << " (idx:" << varIdx << ")";
            } catch (const SymbolTableError& e) {
                throw SemanticError(std::string("Error declaring variable: ") + e.what());
            }
        }
        std::cout << " : " << typeStr << std::endl;
    }
}

void ScopeTypeChecker::visitConstDecl(ConstDeclarationNode* node) {
    std::cout << "[Semantic] Declaring constant " << node->identifier.value 
              << " = " << node->value.value << std::endl;
    
    // Cek apakah sudah dideklarasikan
    if (isDeclaredInCurrentScope(node->identifier.value)) {
        throw SemanticError("Constant '" + node->identifier.value + 
                          "' already declared in current scope");
    }
    
    // Tentukan tipe dari nilai
    BaseType constType = inferTypeFromValue(node->value.value);
    
    // Parse nilai konstanta
    int constValue = 0;
    try {
        constValue = std::stoi(node->value.value);
    } catch (...) {
        constValue = 0;  // Default untuk non-integer
    }
    
    // Insert ke symbol table
    try {
        int constIdx = symbolTable->insert(node->identifier.value, ObjectKind::CONSTANT, 
                                          constType, 0, true, constValue);
        std::cout << "  - Constant '" << node->identifier.value << "' inserted at index " 
                  << constIdx << std::endl;
    } catch (const SymbolTableError& e) {
        throw SemanticError(std::string("Error declaring constant: ") + e.what());
    }
}

void ScopeTypeChecker::visitTypeDecl(TypeDeclarationNode* node) {
    std::cout << "[Semantic] Declaring type " << node->identifier.value << std::endl;
    
    // Cek apakah sudah dideklarasikan
    if (isDeclaredInCurrentScope(node->identifier.value)) {
        throw SemanticError("Type '" + node->identifier.value + 
                          "' already declared in current scope");
    }
    
    BaseType typeCode = BaseType::NOTYPE;
    int ref = 0;
    
    // Tentukan jenis tipe
    if (auto* arrayNode = dynamic_cast<ArrayTypeNode*>(node->pars_type_definition.get())) {
        typeCode = BaseType::ARRAYS;
        ref = processArrayType(arrayNode);
    } else if (auto* typeNode = dynamic_cast<TypeNode*>(node->pars_type_definition.get())) {
        // Alias tipe
        typeCode = getBaseType(typeNode->pars_type_name);
        ref = 0;
    } else {
        throw SemanticError("Unknown type definition for '" + node->identifier.value + "'");
    }
    
    // Insert ke symbol table
    try {
        int typeIdx = symbolTable->insert(node->identifier.value, ObjectKind::TYPE_ID, 
                                         typeCode, ref, true, 0);
        std::cout << "  - Type '" << node->identifier.value << "' inserted at index " 
                  << typeIdx << std::endl;
    } catch (const SymbolTableError& e) {
        throw SemanticError(std::string("Error declaring type: ") + e.what());
    }
}

void ScopeTypeChecker::visitProcedureDecl(ProcedureDeclarationNode* node) {
    std::cout << "[Semantic] Declaring procedure " << node->identifier.value << std::endl;
    
    // Cek apakah sudah dideklarasikan
    if (isDeclaredInCurrentScope(node->identifier.value)) {
        throw SemanticError("Procedure '" + node->identifier.value + 
                          "' already declared in current scope");
    }
    
    // Buat block baru untuk prosedur
    int newBlockIdx = symbolTable->enter_block();
    
    // Insert prosedur ke symbol table
    try {
        int procIdx = symbolTable->insert(node->identifier.value, ObjectKind::PROCEDURE, 
                                         BaseType::NOTYPE, newBlockIdx, true, 0);
        
        std::cout << "  - Procedure '" << node->identifier.value << "' inserted at index " 
                  << procIdx << ", block " << newBlockIdx << std::endl;
        
        // Push scope baru untuk prosedur
        symbolTable->push_scope();
        
        // Process parameters jika ada
        if (node->pars_formal_parameter_list) {
            for (const auto& paramGroup : node->pars_formal_parameter_list->pars_parameter_groups) {
                if (paramGroup && paramGroup->pars_identifier_list) {
                    std::string paramTypeStr;
                    if (auto* typeNode = dynamic_cast<TypeNode*>(paramGroup->pars_type.get())) {
                        paramTypeStr = typeNode->pars_type_name;
                    }
                    BaseType paramType = getBaseType(paramTypeStr);
                    
                    for (const auto& paramName : paramGroup->pars_identifier_list->pars_identifier_list) {
                        symbolTable->insert(paramName, ObjectKind::VARIABLE, 
                                          paramType, 0, true, 0);
                    }
                }
            }
        }
        
        // Process local declarations jika ada
        if (auto* blockNode = dynamic_cast<ProgramNode*>(node->pars_block.get())) {
            if (blockNode->pars_declaration_part) {
                visitDeclarationPart(blockNode->pars_declaration_part.get());
            }
        }
        
        // Pop scope
        symbolTable->pop_scope();
        
    } catch (const SymbolTableError& e) {
        symbolTable->pop_scope();
        throw SemanticError(std::string("Error declaring procedure: ") + e.what());
    }
}

void ScopeTypeChecker::visitFunctionDecl(FunctionDeclarationNode* node) {
    std::cout << "[Semantic] Declaring function " << node->identifier.value << std::endl;
    
    // Cek apakah sudah dideklarasikan
    if (isDeclaredInCurrentScope(node->identifier.value)) {
        throw SemanticError("Function '" + node->identifier.value + 
                          "' already declared in current scope");
    }
    
    // Dapatkan return type
    BaseType returnType = BaseType::NOTYPE;
    if (auto* typeNode = dynamic_cast<TypeNode*>(node->pars_return_type.get())) {
        returnType = getBaseType(typeNode->pars_type_name);
    }
    
    // Buat block baru untuk fungsi
    int newBlockIdx = symbolTable->enter_block();
    
    // Insert fungsi ke symbol table
    try {
        int funcIdx = symbolTable->insert(node->identifier.value, ObjectKind::FUNCTION, 
                                         returnType, newBlockIdx, true, 0);
        
        std::cout << "  - Function '" << node->identifier.value << "' returning " 
                  << static_cast<int>(returnType) << " inserted at index " 
                  << funcIdx << ", block " << newBlockIdx << std::endl;
        
        // Push scope baru untuk fungsi
        symbolTable->push_scope();
        
        // Process parameters jika ada
        if (node->pars_formal_parameter_list) {
            for (const auto& paramGroup : node->pars_formal_parameter_list->pars_parameter_groups) {
                if (paramGroup && paramGroup->pars_identifier_list) {
                    std::string paramTypeStr;
                    if (auto* typeNode = dynamic_cast<TypeNode*>(paramGroup->pars_type.get())) {
                        paramTypeStr = typeNode->pars_type_name;
                    }
                    BaseType paramType = getBaseType(paramTypeStr);
                    
                    for (const auto& paramName : paramGroup->pars_identifier_list->pars_identifier_list) {
                        symbolTable->insert(paramName, ObjectKind::VARIABLE, 
                                          paramType, 0, true, 0);
                    }
                }
            }
        }
        
        // Process local declarations jika ada
        if (auto* blockNode = dynamic_cast<ProgramNode*>(node->pars_block.get())) {
            if (blockNode->pars_declaration_part) {
                visitDeclarationPart(blockNode->pars_declaration_part.get());
            }
        }
        
        // Pop scope
        symbolTable->pop_scope();
        
    } catch (const SymbolTableError& e) {
        symbolTable->pop_scope();
        throw SemanticError(std::string("Error declaring function: ") + e.what());
    }
}

// ============================================================================
// Helper Methods
// ============================================================================

BaseType ScopeTypeChecker::getBaseType(const std::string& typeStr) {
    // Convert ke lowercase
    std::string typeLower = typeStr;
    std::transform(typeLower.begin(), typeLower.end(), typeLower.begin(), ::tolower);
    
    // Cek di mapping
    auto it = typeMap.find(typeLower);
    if (it != typeMap.end()) {
        return it->second;
    }
    
    // Cek apakah tipe custom (user-defined)
    int typeIdx = lookupIdentifier(typeStr);
    if (typeIdx != -1) {
        TabEntry& entry = symbolTable->get_tab(typeIdx);
        if (entry.obj == ObjectKind::TYPE_ID) {
            return entry.typ;
        }
    }
    
    // Default ke NOTYPE jika tidak ditemukan
    std::cerr << "[Warning] Unknown type: " << typeStr << ", using NOTYPE" << std::endl;
    return BaseType::NOTYPE;
}

int ScopeTypeChecker::getTypeSize(BaseType type) {
    // Ukuran default untuk tipe dasar
    switch (type) {
        case BaseType::INTS:
        case BaseType::REALS:
        case BaseType::BOOLS:
        case BaseType::CHARS:
            return 1;
        case BaseType::ARRAYS:
        case BaseType::RECORDS:
            return 1;  // Akan dihitung dari definisi
        default:
            return 1;
    }
}

bool ScopeTypeChecker::isDeclaredInCurrentScope(const std::string& identifier) {
    int idx = symbolTable->lookup_current_scope(identifier);
    return idx != -1;
}

int ScopeTypeChecker::lookupIdentifier(const std::string& identifier) {
    return symbolTable->lookup(identifier);
}

BaseType ScopeTypeChecker::inferTypeFromValue(const std::string& value) {
    // Cek apakah integer
    try {
        std::stoi(value);
        return BaseType::INTS;
    } catch (...) {}
    
    // Cek apakah real
    try {
        std::stof(value);
        return BaseType::REALS;
    } catch (...) {}
    
    // Cek apakah boolean
    std::string valueLower = value;
    std::transform(valueLower.begin(), valueLower.end(), valueLower.begin(), ::tolower);
    if (valueLower == "true" || valueLower == "false" || 
        valueLower == "benar" || valueLower == "salah") {
        return BaseType::BOOLS;
    }
    
    // Cek apakah char atau string
    if (value.length() >= 2 && value[0] == '\'' && value[value.length()-1] == '\'') {
        if (value.length() == 3) {
            return BaseType::CHARS;  // Single character
        }
    }
    
    return BaseType::NOTYPE;
}

int ScopeTypeChecker::processArrayType(const ArrayTypeNode* arrayDef) {
    // Dapatkan informasi array
    BaseType indexType = BaseType::INTS;  // Default
    BaseType elementType = BaseType::NOTYPE;
    
    if (auto* typeNode = dynamic_cast<TypeNode*>(arrayDef->pars_type.get())) {
        elementType = getBaseType(typeNode->pars_type_name);
    }
    
    // Dapatkan range
    int low = 0;
    int high = 0;
    
    if (arrayDef->pars_range) {
        // Extract low and high from range - simplified version
        // Dalam implementasi lengkap, perlu evaluasi expression
        low = 0;
        high = 10;  // Default range
    }
    
    int elSize = getTypeSize(elementType);
    
    // Insert ke atab
    int arrayIdx = symbolTable->enter_array(indexType, elementType, 0, 
                                           low, high, elSize);
    
    return arrayIdx;
}
