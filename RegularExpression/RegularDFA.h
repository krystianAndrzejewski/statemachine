#pragma once
#include "NFA.h"
#include <set>
#include <map>
class RegularDFA
{
public:
    RegularDFA();
    ~RegularDFA();

    void createFromInitialNfa(const NFA &nfa);
    void createFromBackwardInitialNfa(const NFA &nfa);

    bool returnNextState(char action, std::size_t &nextStateNumber);
    std::size_t getActualState()  const { return actualState; }
    std::size_t getStartState()  const { return initialState; }
    const std::unordered_set<std::size_t>& getFinalStates()  const { return finalStates; }
    const std::unordered_map<std::size_t, std::set<std::set<std::size_t>>> &getMapConectedNFAStates() { return mapConnectedNFAStates; }
    std::size_t getSize()  const { return states.size(); }
    void returnToStartPosition() { actualState = initialState; }
    void returnMovePossibility(
        const std::size_t &number,
        std::unordered_map<char, std::size_t> &pNextStates) const;
    void returnNFAStates(
        const std::size_t &number,
        std::set<std::size_t> &NFAstates) const;

    void print(std::string &output) const;

private:

    std::vector<State *> states;
    std::unordered_map<const State *, std::size_t> mapStates;
    std::unordered_map<const State *, std::set<std::size_t>> mapNFAStates;
    std::map<std::set<std::size_t>, const State *> mapRDFAfromNFAStates;
    std::unordered_map<std::size_t, std::set<std::set<std::size_t>>> mapConnectedNFAStates;
    std::unordered_set<std::size_t> finalStates;
    std::size_t initialState;
    std::size_t actualState;
};

