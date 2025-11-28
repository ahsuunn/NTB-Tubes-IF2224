#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

struct PairHash
{
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2> &p) const noexcept
    {
        std::hash<T1> h1;
        std::hash<T2> h2;
        return (h1(p.first) * 1315423911u) ^ h2(p.second);
    }
};

class DFA
{
public:
    DFA() = default;
    DFA(std::string start_state,
        std::unordered_set<std::string> final_states,
        std::unordered_map<std::pair<std::string, std::string>, std::string, PairHash> transitions)
        : start_state(std::move(start_state)), final_states(std::move(final_states)), transitions(std::move(transitions)) {}

    const std::string &start() const { return start_state; }
    const std::unordered_set<std::string> &finals() const { return final_states; }

    // returns next state or empty string if not found
    std::string next_state(const std::string &state, const std::string &inp) const;

private:
    std::string start_state;
    std::unordered_set<std::string> final_states;
    std::unordered_map<std::pair<std::string, std::string>, std::string, PairHash> transitions; // (state, inp) -> next
};

// Loaders
DFA load_dfa_txt(const std::string &path);
DFA load_dfa_json(const std::string &path);
