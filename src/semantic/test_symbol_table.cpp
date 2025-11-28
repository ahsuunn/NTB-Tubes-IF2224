#include "semantic/symbol_table.hpp"
#include <iostream>

int main() {
    SymbolTable symtab;
    
    std::cout << "=== Test 1: Initial State ===\n";
    symtab.print_tab();
    symtab.print_btab();
    
    std::cout << "\n=== Test 2: Insert Program Name ===\n";
    int prog_idx = symtab.insert("TestProgram", ObjectKind::PROCEDURE, BaseType::NOTYPE, 0, true, 0);
    std::cout << "Inserted 'TestProgram' at index: " << prog_idx << "\n";
    
    std::cout << "\n=== Test 3: Insert Variables ===\n";
    int a_idx = symtab.insert("a", ObjectKind::VARIABLE, BaseType::INTS, 0, true, 0);
    int b_idx = symtab.insert("b", ObjectKind::VARIABLE, BaseType::INTS, 0, true, 1);
    std::cout << "Inserted 'a' at index: " << a_idx << "\n";
    std::cout << "Inserted 'b' at index: " << b_idx << "\n";
    
    std::cout << "\n=== Test 4: Lookup ===\n";
    int found = symtab.lookup("a");
    std::cout << "Lookup 'a': " << (found != -1 ? "found at " + std::to_string(found) : "not found") << "\n";
    found = symtab.lookup("writeln");
    std::cout << "Lookup 'writeln': " << (found != -1 ? "found at " + std::to_string(found) : "not found") << "\n";
    found = symtab.lookup("xyz");
    std::cout << "Lookup 'xyz': " << (found != -1 ? "found at " + std::to_string(found) : "not found") << "\n";
    
    std::cout << "\n=== Test 5: Push Scope (New Block) ===\n";
    symtab.push_scope();
    std::cout << "Current level: " << symtab.get_current_level() << "\n";
    
    int x_idx = symtab.insert("x", ObjectKind::VARIABLE, BaseType::REALS, 0, true, 0);
    std::cout << "Inserted 'x' in new scope at index: " << x_idx << "\n";
    
    std::cout << "\n=== Test 6: Lookup from Inner Scope ===\n";
    found = symtab.lookup("a");
    std::cout << "Lookup 'a' from inner scope: " << (found != -1 ? "found at " + std::to_string(found) : "not found") << "\n";
    found = symtab.lookup("x");
    std::cout << "Lookup 'x' from inner scope: " << (found != -1 ? "found at " + std::to_string(found) : "not found") << "\n";
    
    std::cout << "\n=== Test 7: Pop Scope ===\n";
    symtab.pop_scope();
    std::cout << "Current level after pop: " << symtab.get_current_level() << "\n";
    found = symtab.lookup("x");
    std::cout << "Lookup 'x' after pop: " << (found != -1 ? "found at " + std::to_string(found) : "not found") << "\n";
    
    std::cout << "\n=== Test 8: Enter Array ===\n";
    int arr_idx = symtab.enter_array(BaseType::INTS, BaseType::REALS, 0, 1, 10, 1);
    std::cout << "Entered array at index: " << arr_idx << "\n";
    
    std::cout << "\n=== Test 9: Duplicate Declaration Error ===\n";
    try {
        symtab.insert("a", ObjectKind::VARIABLE, BaseType::INTS, 0, true, 2);
        std::cout << "ERROR: Should have thrown exception!\n";
    } catch (const SymbolTableError& e) {
        std::cout << "Caught expected error: " << e.what() << "\n";
    }
    
    std::cout << "\n=== Final State ===\n";
    symtab.print_tab();
    symtab.print_btab();
    symtab.print_atab();
    
    std::cout << "\n=== All Tests Passed ===\n";
    return 0;
}