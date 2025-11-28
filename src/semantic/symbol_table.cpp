#include "semantic/symbol_table.hpp"
#include <iostream>
#include <iomanip>

SymbolTable::SymbolTable() : level(0), t(0), b(0), a(0) {
    display.push_back(0);
    init_standard_types();
    // Standard procedures like write, writeln, read, readln will be added when first used
    // init_standard_procedures();
}

void SymbolTable::init_standard_types() {
    // Reserve space for reserved words (0-28)
    std::vector<std::string> reserved_words = {
        "program", "variabel", "mulai", "selesai", "const", "tipe",
        "prosedur", "fungsi", "jika", "maka", "selainitu", "untuk",
        "ke", "turun", "lakukan", "selama", "ulangi", "sampai",
        "larik", "dari", "integer", "real", "boolean", "char",
        "and", "or", "not", "div", "mod"
    };
    
    for (const auto& word : reserved_words) {
        TabEntry entry;
        entry.name = word;
        entry.link = 0;
        entry.obj = ObjectKind::CONSTANT;  // Reserved words as constants
        entry.typ = BaseType::NOTYPE;
        entry.ref = 0;
        entry.normal = true;
        entry.lev = 0;
        entry.adr = 0;
        tab.push_back(entry);
    }
    t = tab.size();
    
    BTabEntry global_block;
    global_block.last = 0;
    global_block.lastpar = 0;
    global_block.psize = 0;
    global_block.vsize = 0;
    btab.push_back(global_block);
    b = 1;
}

void SymbolTable::init_standard_procedures() {
    insert("write", ObjectKind::PROCEDURE, BaseType::NOTYPE, 0, true, 0);
    insert("writeln", ObjectKind::PROCEDURE, BaseType::NOTYPE, 0, true, 0);
    insert("read", ObjectKind::PROCEDURE, BaseType::NOTYPE, 0, true, 0);
    insert("readln", ObjectKind::PROCEDURE, BaseType::NOTYPE, 0, true, 0);
}

int SymbolTable::insert(const std::string& name, ObjectKind obj, BaseType typ, int ref, bool normal, int adr) {
    if (lookup_current_scope(name) != -1) {
        throw SymbolTableError("Identifier '" + name + "' already declared in current scope");
    }
    
    TabEntry entry;
    entry.name = name;
    entry.link = btab[display[level]].last;
    entry.obj = obj;
    entry.typ = typ;
    entry.ref = ref;
    entry.normal = normal;
    entry.lev = level;
    entry.adr = adr;
    
    tab.push_back(entry);
    int idx = tab.size() - 1;
    btab[display[level]].last = idx;
    t = tab.size();
    
    return idx;
}

int SymbolTable::lookup(const std::string& name) {
    // Auto-insert standard procedures if not found
    std::vector<std::string> std_procs = {"write", "writeln", "read", "readln"};
    bool is_std_proc = false;
    for (const auto& proc : std_procs) {
        if (name == proc) {
            is_std_proc = true;
            break;
        }
    }
    
    for (int l = level; l >= 0; l--) {
        int i = btab[display[l]].last;
        while (i > 0) {
            if (tab[i].name == name) {
                return i;
            }
            i = tab[i].link;
        }
    }
    
    // If not found and it's a standard procedure, add it now
    if (is_std_proc) {
        try {
            return insert(name, ObjectKind::PROCEDURE, BaseType::NOTYPE, 0, true, 0);
        } catch (...) {
            return -1;
        }
    }
    return -1;
}

int SymbolTable::lookup_current_scope(const std::string& name) {
    int i = btab[display[level]].last;
    while (i > 0) {
        if (tab[i].name == name) {
            return i;
        }
        i = tab[i].link;
    }
    return -1;
}

void SymbolTable::push_scope() {
    level++;
    int block_idx = enter_block();
    if (level >= (int)display.size()) {
        display.push_back(block_idx);
    } else {
        display[level] = block_idx;
    }
}

void SymbolTable::pop_scope() {
    if (level > 0) {
        level--;
    }
}

int SymbolTable::enter_block() {
    BTabEntry entry;
    entry.last = 0;
    entry.lastpar = 0;
    entry.psize = 0;
    entry.vsize = 0;
    
    btab.push_back(entry);
    b = btab.size();
    return btab.size() - 1;
}

void SymbolTable::set_block_params(int block_idx, int lastpar, int psize) {
    if (block_idx >= 0 && block_idx < (int)btab.size()) {
        btab[block_idx].lastpar = lastpar;
        btab[block_idx].psize = psize;
    }
}

void SymbolTable::set_block_vars(int block_idx, int vsize) {
    if (block_idx >= 0 && block_idx < (int)btab.size()) {
        btab[block_idx].vsize = vsize;
    }
}

int SymbolTable::enter_array(BaseType inxtyp, BaseType eltyp, int elref, int low, int high, int elsize) {
    ATabEntry entry;
    entry.inxtyp = inxtyp;
    entry.eltyp = eltyp;
    entry.elref = elref;
    entry.low = low;
    entry.high = high;
    entry.elsize = elsize;
    entry.size = (high - low + 1) * elsize;
    
    atab.push_back(entry);
    a = atab.size();
    return atab.size() - 1;
}

TabEntry& SymbolTable::get_tab(int idx) {
    if (idx < 0 || idx >= (int)tab.size()) {
        throw SymbolTableError("Invalid tab index: " + std::to_string(idx));
    }
    return tab[idx];
}

BTabEntry& SymbolTable::get_btab(int idx) {
    if (idx < 0 || idx >= (int)btab.size()) {
        throw SymbolTableError("Invalid btab index: " + std::to_string(idx));
    }
    return btab[idx];
}

ATabEntry& SymbolTable::get_atab(int idx) {
    if (idx < 0 || idx >= (int)atab.size()) {
        throw SymbolTableError("Invalid atab index: " + std::to_string(idx));
    }
    return atab[idx];
}

void SymbolTable::print_tab() const {
    std::cout << "\n=== TAB (Identifier Table) ===\n";
    std::cout << std::setw(4) << "idx" 
              << std::setw(15) << "name" 
              << std::setw(6) << "link" 
              << std::setw(12) << "obj" 
              << std::setw(8) << "typ" 
              << std::setw(6) << "ref" 
              << std::setw(6) << "nrm" 
              << std::setw(6) << "lev" 
              << std::setw(6) << "adr" << "\n";
    std::cout << std::string(69, '-') << "\n";
    
    for (size_t i = 0; i < tab.size(); i++) {
        const TabEntry& e = tab[i];
        std::string obj_str;
        switch (e.obj) {
            case ObjectKind::CONSTANT: obj_str = "constant"; break;
            case ObjectKind::VARIABLE: obj_str = "variable"; break;
            case ObjectKind::TYPE_ID: obj_str = "type"; break;
            case ObjectKind::PROCEDURE: obj_str = "procedure"; break;
            case ObjectKind::FUNCTION: obj_str = "function"; break;
        }
        
        std::cout << std::setw(4) << i
                  << std::setw(15) << e.name
                  << std::setw(6) << e.link
                  << std::setw(12) << obj_str
                  << std::setw(8) << static_cast<int>(e.typ)
                  << std::setw(6) << e.ref
                  << std::setw(6) << (e.normal ? 1 : 0)
                  << std::setw(6) << e.lev
                  << std::setw(6) << e.adr << "\n";
    }
}

void SymbolTable::print_btab() const {
    std::cout << "\n=== BTAB (Block Table) ===\n";
    std::cout << std::setw(4) << "idx"
              << std::setw(8) << "last"
              << std::setw(8) << "lpar"
              << std::setw(8) << "psize"
              << std::setw(8) << "vsize" << "\n";
    std::cout << std::string(36, '-') << "\n";
    
    for (size_t i = 0; i < btab.size(); i++) {
        const BTabEntry& e = btab[i];
        std::cout << std::setw(4) << i
                  << std::setw(8) << e.last
                  << std::setw(8) << e.lastpar
                  << std::setw(8) << e.psize
                  << std::setw(8) << e.vsize << "\n";
    }
}

void SymbolTable::print_atab() const {
    std::cout << "\n=== ATAB (Array Table) ===\n";
    std::cout << std::setw(4) << "idx"
              << std::setw(8) << "xtyp"
              << std::setw(8) << "etyp"
              << std::setw(8) << "eref"
              << std::setw(8) << "low"
              << std::setw(8) << "high"
              << std::setw(8) << "elsz"
              << std::setw(8) << "size" << "\n";
    std::cout << std::string(60, '-') << "\n";
    
    for (size_t i = 0; i < atab.size(); i++) {
        const ATabEntry& e = atab[i];
        std::cout << std::setw(4) << i
                  << std::setw(8) << static_cast<int>(e.inxtyp)
                  << std::setw(8) << static_cast<int>(e.eltyp)
                  << std::setw(8) << e.elref
                  << std::setw(8) << e.low
                  << std::setw(8) << e.high
                  << std::setw(8) << e.elsize
                  << std::setw(8) << e.size << "\n";
    }
}