#pragma once
#include "DeterministicState.h"
#include <map>

class InitialDFA
{
public:
    InitialDFA();
    ~InitialDFA();

    void concatenation(InitialDFA& consequent);
    void transition(const std::vector<char> &characterSet);
    void alternative(InitialDFA& other);
    void greedyRepeation();
    void lazyRepeation();
    void greedyRepeationPlus();
    void lazyRepeationPlus();
    void zeroOrOne();
    void group();

    bool returnNextState(char action, std::size_t &nextStateNumber);
    bool setState(std::size_t state);
    std::size_t getActualState(State::StateType &pType)  const { pType = states[actualState]->returnType(); return actualState; }
    std::size_t getStartState(State::StateType &pType)  const { pType = states[initialState]->returnType(); return initialState; }
    std::size_t getFinalState()  const { return finalState; }
    std::size_t getSize()  const { return states.size(); }
    void returnToStartPosition() { actualState = initialState; }
    void returnMovePossibility(
        const std::size_t &number,
        std::unordered_map<char, std::pair<std::size_t, State::StateType>> &pNextStates) const;
    void getGroups(std::map<std::size_t, std::size_t> &groups) const;
    bool getType(std::size_t number, State::StateType& type) const;
    std::size_t getNumberOfGroups() const;

    void print(std::string &output) const;

private:
    void createNewState(DeterministicState::DetermisitcStateType type);
    void addNewState(State *state);
    std::vector<State *> states;
    std::unordered_map<const State *, std::size_t> mapStates;
    std::size_t finalState;
    std::size_t initialState;
    std::size_t actualState;
};

