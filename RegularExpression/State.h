#pragma once
#include <unordered_map>
#include <unordered_set>

class State
{
public:

    enum StateType
    {
        Deterministic,
        NonDeterministic,
        Priority,
        Entry,
        Exit
    };

    State();
    virtual ~State();
    virtual bool addNextState(char key, State *nextState);
    virtual bool addNextState(State *nextState);
    void removeConnection();
    void swapInNextStates(State* state);
    void swapInPreviousStates(State* state);
    StateType returnType() const;

    const std::unordered_map<char, std::unordered_set<const State *>> &getNextStates() const;
    const std::unordered_map<char, std::unordered_set<const State *>> &getPreviousStates() const;

protected:
    State(StateType pType);
    StateType type;
private:
    bool removeStateFromPrevious(const State *neighbour);
    bool removeStateFromNext(const State *neighbour);
    void addPreviousState(const char key, const State* previousState);
    std::unordered_map<char, std::unordered_set<const State *>> nextStates;
    std::unordered_map<char, std::unordered_set<const State *>> previousStates;

};

