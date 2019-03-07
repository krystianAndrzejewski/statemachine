#include "DeterministicState.h"

template<typename T>
DeterministicState<T>::DeterministicState()
{
}

template<typename T>
DeterministicState<T>::~DeterministicState()
{
}

template<typename T>
bool DeterministicState<T>::addNextState(const T &key, DeterministicState<T> *nextState)
{
    if (nextStates.find(key) == nextStates.end())
    {
        nextStates[key] = nextState;
        nextState->addPreviousState(key, this);
        return true;
    }
    else
    {
        return false;
    }
}

template<typename T>
const DeterministicState<T> *DeterministicState<T>::getNextState(const T &key)
{
    if (nextStates.find(key) != nextStates.end())
    {
        return nextStates[key];
    }
    else
    {
        return nullptr;
    }
}

template<typename T>
const std::unordered_map<T, const DeterministicState<T> *> &DeterministicState<T>::getNextStates()
{
    return nextStates;
}

template<typename T>
const std::unordered_set<const DeterministicState<T> *> *DeterministicState<T>::getPreviousState(const T &key)
{
    if (previousStates.find(key) != previousStates.end())
    {
        return &(previousStates[key]);
    }
    else
    {
        return nullptr;
    }
}

template<typename T>
const std::unordered_map<T, std::unordered_set<const DeterministicState<T> *>> &DeterministicState<T>::getPreviousStates()
{
    return previousStates;
}

template<typename T>
void DeterministicState<T>::addPreviousState(const T &key, const DeterministicState<T>* previousState)
{
    previousStates[key].insert(previousState);
}

template DeterministicState<char>;
