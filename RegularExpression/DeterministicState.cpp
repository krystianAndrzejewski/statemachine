#include "DeterministicState.h"


DeterministicState::DeterministicState(DetermisitcStateType pType) : State(static_cast<StateType>(pType)), connectedState(nullptr)
{
}


DeterministicState::~DeterministicState()
{
}

bool DeterministicState::addNextState(char key, State *nextState)
{
    switch (returnType())
    {
    case State::Deterministic:
        if (getNextStates().find(key) == getNextStates().end() || getNextStates().at(key).empty())
        {
            return State::addNextState(key, nextState);
        }
        break;
    case State::Priority:
        if ((key == '+' ||
            key == '-') &&
            (getNextStates().find(key) == getNextStates().end() || getNextStates().at(key).empty()))
        {
            return State::addNextState(key, nextState);
        }
        break;
    case State::Entry:
    case State::Exit:
        if (key == ' ' && (getNextStates().empty() || getNextStates().at(key).empty()))
        {
            return State::addNextState(key, nextState);
        }
        break;
    default:
        break;
    }
    return false;
}

bool DeterministicState::addNextState(State *nextState)
{
    switch (returnType())
    {
    case State::Entry:
    case State::Exit:
        if (getNextStates().empty())
        {
            return State::addNextState(' ', nextState);
        }
        break;
    default:
        break;
    }
    return false;
}

bool DeterministicState::connectGroupState(DeterministicState* state)
{
    switch (returnType())
    {
    case State::Entry:
        if (state->returnType() == State::Exit && connectedState == nullptr)
        {
            connectedState = state;
            connectedState = state->connectGroupState(this) ? connectedState : nullptr;
            return connectedState ? true : false;
        }
        break;
    case State::Exit:
        if (state->returnType() == State::Entry && connectedState == nullptr)
        {
            connectedState = state;
            connectedState = state->connectGroupState(this) ? connectedState : nullptr;
            return connectedState ? true : false;
        }
        break;
    default:
        break;
    }
    return state == connectedState ? true : false;
}

bool DeterministicState::divideGroupState(DeterministicState* state)
{
    if (connectedState == state)
    {
        connectedState = nullptr;
        state->connectedState = nullptr;
        return true;
    }
    return false;
}

bool DeterministicState::changeTypeState(DetermisitcStateType pType)
{
    if (getNextStates().empty() && connectedState == nullptr)
    {
        type = static_cast<StateType>(pType);
        return true;
    }
    return false;
}