#include "parser/parser.hpp"
#include <fstream>
#include <filesystem>

class Utils{
    public:
        static std::string resolve_from_here(const std::string& p);
        static void print_token(const std::string& type, const std::string& value, const std::string& prefix, bool is_last);
        static void print_parse_tree(const ParseTreeNode* node, const std::string& prefix = "", bool is_last = true, bool is_root = true);
    };
    