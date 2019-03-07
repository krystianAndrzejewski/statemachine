#pragma once
#include "DeterministicState.h"
#include <vector>

template<typename T>
class DeterministicFiniteAutomata
{
public:
    DeterministicFiniteAutomata(std::size_t size, std::size_t pInitialState, const std::vector<std::size_t> &pFinalStates);
    ~DeterministicFiniteAutomata();
    bool linkStates(std::size_t precedessor, std::size_t consequent, const T condition);
    void simplifyDFA();
    bool returnNextState(const T action, std::size_t &nextStateNumber);

private:

    void findReachableStates();
    void HopcroftAlgorithm(std::vector<std::unordered_set<std::size_t>> &statesGroups);
    void checkPreviousStateForStateSet(
        std::unordered_set<std::size_t> &actualStates,
        std::unordered_map<T, std::unordered_set<std::size_t>> &previousStates);
    void divideStatesGroups(
        std::vector<std::unordered_set<std::size_t>> &statesGroups,
        std::unordered_map<T, std::unordered_set<std::size_t>> &previousStatesDictionary,
        std::unordered_set<std::size_t> &processedSets);
    void createSimplifiedStateMachine(std::vector<std::unordered_set<std::size_t>> &statesGroups);

    std::unordered_set<std::size_t> reachableStates;
    std::size_t actualState;
    std::vector<DeterministicState<T>> states;
    std::unordered_map<const DeterministicState<T> *, std::size_t> mapStates;
    std::size_t initialState;
    std::unordered_set<std::size_t> finalStates;
};

