#include "State.h"


State::State(StateType pType) : type(pType)
{
}

State::State() : type(NonDeterministic)
{
}

State::~State()
{
}

bool State::addNextState(const char key, State *nextState)
{
    if (nextState)
    {
        nextStates[key].insert(nextState);
        nextState->addPreviousState(key, this);
        return true;
    }
    return false;
}

bool State::addNextState(State *nextState)
{
    return false;
}

const std::unordered_map<char, std::unordered_set<const State *>> &State::getNextStates() const
{
    return nextStates;
}

const std::unordered_map<char, std::unordered_set<const State *>> &State::getPreviousStates() const
{
    return previousStates;
}

void State::addPreviousState(const char key, const State* previousState)
{
    previousStates[key].insert(previousState);
}

State::StateType State::returnType() const
{
    return type;
}

bool State::removeStateFromPrevious(const State *neighbour)
{
    bool success = false;
    for (auto &partOfPreviousStates : previousStates)
    {
        if (partOfPreviousStates.second.find(neighbour) != partOfPreviousStates.second.end())
        {
            partOfPreviousStates.second.erase(neighbour);
            success = true;
        }
    }
    return success;
}

bool State::removeStateFromNext(const State *neighbour)
{
    bool success = false;
    for (auto &partOfNextStates : nextStates)
    {
        if (partOfNextStates.second.find(neighbour) != partOfNextStates.second.end())
        {
            partOfNextStates.second.erase(neighbour);
            success = true;
        }
    }
    return success;
}

void State::removeConnection()
{
    swapInNextStates(nullptr);
    swapInPreviousStates(nullptr);
}

void State::swapInNextStates(State *pState)
{
    for (auto partOfNextStates : nextStates)
    {
        for (auto state : partOfNextStates.second)
        {
            const_cast<State*>(state)->removeStateFromPrevious(this);
            if (pState)
            {
                pState->addNextState(partOfNextStates.first, const_cast<State*>(state));
            }
            removeStateFromNext(state);
        }
    }
    nextStates.clear();
}

void State::swapInPreviousStates(State *pState)
{
    for (auto &partOfPreviousStates : previousStates)
    {
        for (auto &state : partOfPreviousStates.second)
        {
            const_cast<State*>(state)->removeStateFromNext(this);
            if (pState)
            {
                const_cast<State*>(state)->addNextState(partOfPreviousStates.first, pState);
            }
        }
    }
    previousStates.clear();
}