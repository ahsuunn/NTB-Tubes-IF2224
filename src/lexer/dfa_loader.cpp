#include "lexer/dfa_loader.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cctype>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::string DFA::next_state(const std::string& state, const std::string& inp) const {
    auto it = transitions.find({state, inp});
    if (it == transitions.end()) return std::string();
    return it->second;
}

DFA load_dfa_txt(const std::string& path) {
    std::ifstream f(path);
    if (!f) throw std::runtime_error("Cannot open DFA txt: " + path);

    std::string start_state; bool has_start=false;
    std::unordered_set<std::string> finals;
    std::unordered_map<std::pair<std::string,std::string>, std::string, PairHash> trans;

    std::string raw;
    while (std::getline(f, raw)) {
        std::string line = raw;
        size_t i=0; while (i<line.size() && std::isspace((unsigned char)line[i])) ++i;
        line = line.substr(i);
        if (line.empty() || line[0]=='#') continue;

        std::string lower = line;
        for (char &c : lower) c = (char)std::tolower((unsigned char)c);

        if (lower.rfind("start_state", 0) == 0) {
            auto pos = line.find('=');
            if (pos==std::string::npos) throw std::runtime_error("Invalid start_state line: "+line);
            start_state = std::string(line.begin()+pos+1, line.end());
            size_t s=0; while (s<start_state.size() && std::isspace((unsigned char)start_state[s])) ++s;
            start_state = start_state.substr(s);
            has_start=true;
        } else if (lower.rfind("final_state", 0) == 0) {
            auto pos = line.find('=');
            if (pos==std::string::npos) throw std::runtime_error("Invalid final_state line: "+line);
            std::string rhs(line.begin()+pos+1, line.end());
            std::stringstream ss(rhs);
            std::string item;
            while (std::getline(ss, item, ',')) {
                size_t s=0; while (s<item.size() && std::isspace((unsigned char)item[s])) ++s;
                item = item.substr(s);
                if (!item.empty()) finals.insert(item);
            }
        } else {
            std::stringstream ss(line);
            std::string src, inp, dst;
            if (!(ss >> src >> inp >> dst)) throw std::runtime_error("Invalid transition line: "+line);
            trans[{src, inp}] = dst;
        }
    }
    if (!has_start) throw std::runtime_error("Start_state not defined in DFA .txt");
    return DFA(start_state, finals, trans);
}

DFA load_dfa_json(const std::string& path) {
    std::ifstream f(path);
    if (!f) throw std::runtime_error("Cannot open DFA json: " + path);
    json data; f >> data;

    std::string start_state = data.at("start_state").get<std::string>();
    std::unordered_set<std::string> finals;
    for (const auto& s : data.at("final_states")) finals.insert(s.get<std::string>());

    std::unordered_map<std::pair<std::string,std::string>, std::string, PairHash> trans;
    const auto& trans_obj = data.at("transitions");
    for (auto it = trans_obj.begin(); it != trans_obj.end(); ++it) {
        const std::string src = it.key();
        const auto& mapping = it.value();
        for (auto it2 = mapping.begin(); it2 != mapping.end(); ++it2) {
            trans[{src, it2.key()}] = it2.value().get<std::string>();
        }
    }
    return DFA(start_state, finals, trans);
}
