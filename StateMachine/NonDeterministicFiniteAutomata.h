#pragma once
#include "NonDeterministicState.h"

template<typename T>
class DeterministicFiniteAutomata;

template<typename T>
class NonDeterministicFiniteAutomata
{
public:
    NonDeterministicFiniteAutomata(std::size_t size, std::size_t pInitialState, const std::vector<std::size_t> &pFinalStates);
    ~NonDeterministicFiniteAutomata();
    DeterministicFiniteAutomata<T> *createDFA();
    void linkStates(std::size_t precedessor, std::size_t consequent);
    void linkStates(std::size_t precedessor, std::size_t consequent, const T condition);
private:
    void findReachableStates();
    std::unordered_set<std::size_t> reachableStates;
    std::unordered_set<std::size_t> actualState;
    std::vector<NonDeterministicState<T>> states;
    std::unordered_map<const NonDeterministicState<T> *, std::size_t> mapStates;
    std::size_t initialState;
    std::unordered_set<std::size_t> finalStates;
};

