#pragma once
#include "InitialDFA.h"
#include "RegularDFA.h"
#include "GroupState.h"

class GroupDFA
{
public:
    GroupDFA();
    ~GroupDFA();
    void createFromInitialDFA(const InitialDFA& initialDfa, RegularDFA &intvertedRegularDfa);

    bool returnNextState(
        const std::set<std::size_t> &action,
        std::size_t &nextStateNumber,
        std::vector<std::size_t> &groupNumbers);
    std::size_t getActualState()  const { return actualState; }
    std::size_t getStartState()  const { return initialState; }
    std::size_t getFinalStates()  const { return finalState; }
    std::size_t getSize()  const { return states.size(); }
    void returnToStartPosition() { actualState = initialState; }
    void returnMovePossibility(
        const std::size_t &number,
        std::map<const std::set<std::size_t>, std::size_t> &pNextStates,
        std::vector<std::size_t> &groupNumbers) const;

    void print(std::string &output) const;
private:
    void createGroupDfa(const InitialDFA& initialDfa, RegularDFA &regualarDfa);
    std::vector<GroupState *> states;
    std::unordered_map<const GroupState *, std::size_t> mapStates;
    std::map<std::size_t, std::size_t> groupsMap;
    std::size_t initialState;
    std::size_t finalState;
    std::size_t actualState;
};

