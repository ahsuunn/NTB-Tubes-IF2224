#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>

enum class ObjectKind {
    CONSTANT,
    VARIABLE,
    TYPE_ID,
    PROCEDURE,
    FUNCTION
};

enum class BaseType {
    NOTYPE = 0,
    INTS = 1,
    REALS = 2,
    BOOLS = 3,
    CHARS = 4,
    ARRAYS = 5,
    RECORDS = 6
};

struct TabEntry {
    std::string name;
    int link;
    ObjectKind obj;
    BaseType typ;
    int ref;
    bool normal;
    int lev;
    int adr;
};

struct BTabEntry {
    int last;
    int lastpar;
    int psize;
    int vsize;
};

struct ATabEntry {
    BaseType inxtyp;
    BaseType eltyp;
    int elref;
    int low;
    int high;
    int elsize;
    int size;
};

class SymbolTableError : public std::runtime_error {
public:
    explicit SymbolTableError(const std::string& msg) : std::runtime_error(msg) {}
};

class SymbolTable {
public:
    SymbolTable();
    
    void init_standard_types();
    void init_standard_procedures();
    
    int insert(const std::string& name, ObjectKind obj, BaseType typ, int ref, bool normal, int adr);
    int lookup(const std::string& name);
    int lookup_current_scope(const std::string& name);
    
    void push_scope();
    void pop_scope();
    
    int enter_block();
    void set_block_params(int block_idx, int lastpar, int psize);
    void set_block_vars(int block_idx, int vsize);
    
    int enter_array(BaseType inxtyp, BaseType eltyp, int elref, int low, int high, int elsize);
    
    TabEntry& get_tab(int idx);
    BTabEntry& get_btab(int idx);
    ATabEntry& get_atab(int idx);
    
    int get_current_level() const { return level; }
    int get_tab_size() const { return tab.size(); }
    int get_btab_size() const { return btab.size(); }
    int get_atab_size() const { return atab.size(); }
    
    void print_tab() const;
    void print_btab() const;
    void print_atab() const;

private:
    std::vector<TabEntry> tab;
    std::vector<BTabEntry> btab;
    std::vector<ATabEntry> atab;
    
    std::vector<int> display;
    int level;
    
    int t;
    int b;
    int a;
};

#endif