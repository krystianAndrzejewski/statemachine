#pragma once
#include "State.h"
#include "InitialDFA.h"
#include <set>

class NFA
{
public:
    NFA();
    ~NFA();

    void createFromInitialDfa(const InitialDFA &dfa);

    bool returnNextState(char action, std::set<std::size_t > &nextStateNumber);
    const std::set<std::size_t>& getActualState()  const { return actualStates; }
    const std::set<std::size_t>& getStartStates()  const { return initialStates; }
    const std::set<std::size_t>& getFinalStates()  const { return finalStates; }
    std::size_t getSize()  const { return states.size(); }
    void returnToStartPosition() { actualStates.insert(initialStates.begin(), initialStates.end()); }
    void returnMovePossibility(
        const std::set<std::size_t > &numbers,
        std::unordered_map<char, std::set<std::size_t>> &pNextStates) const;

    void returnPreviousStates(
        const std::set<std::size_t > &numbers,
        std::unordered_map<char, std::set<std::size_t>> &pPreviousStates) const;

    void print(std::string &output) const;

private:
    std::vector<State *> states;
    std::unordered_map <const State *, std::size_t> mapState;
    std::set<std::size_t> initialStates;
    std::set<std::size_t> finalStates;
    std::set<std::size_t> actualStates;

};

