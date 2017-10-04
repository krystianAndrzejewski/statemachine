#pragma once
#include "State.h"
class DeterministicState :
    public State
{
public:
    enum DetermisitcStateType
    {
        Simple = State::Deterministic,
        Entry = State::Entry,
        Exit = State::Exit,
        Priority = State::Priority
    };
    DeterministicState(DetermisitcStateType pType);
    ~DeterministicState();
    bool addNextState(char key, State *nextState);
    bool addNextState(State *nextState);
    bool connectGroupState(DeterministicState* state);
    bool divideGroupState(DeterministicState* state);
    bool changeTypeState(DetermisitcStateType pType);
    const DeterministicState *getConnectedState(){ return connectedState; }
private:
    const DeterministicState *connectedState;
};

