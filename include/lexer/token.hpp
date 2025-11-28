#pragma once
#include <string>
#include <sstream>

struct Token
{
    std::string type;
    std::string value;
    int line;
    int column;

    std::string toString() const
    {
        std::ostringstream oss;
        oss << type << "(" << value << ")";
        return oss.str();
    }
};
