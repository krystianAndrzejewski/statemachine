#include "NonDeterministicState.h"

template<typename T>
NonDeterministicState<T>::NonDeterministicState()
{
}

template<typename T>
NonDeterministicState<T>::~NonDeterministicState()
{
}

template<typename T>
void NonDeterministicState<T>::addNextState(const T &key, NonDeterministicState<T> *nextState)
{
    nextStates[key].insert(nextState);
    nextState->addPreviousState(key, this);
}

template<typename T>
void NonDeterministicState<T>::addNextEpsilonState(NonDeterministicState<T> *nextState)
{
    nextEpsilonStates.insert(nextState);
}

template<typename T>
const std::unordered_set<const NonDeterministicState<T> *> *NonDeterministicState<T>::getNextState(const T &key) const
{
    if (nextStates.find(key) != nextStates.end())
    {
        return &(nextStates.at(key));
    }
    else
    {
        return nullptr;
    }
}

template<typename T>
const std::unordered_map<T, std::unordered_set<const NonDeterministicState<T> *>> &NonDeterministicState<T>::getNextStates() const
{
    return nextStates;
}

template<typename T>
const std::unordered_set<const NonDeterministicState<T> *> &NonDeterministicState<T>::getPreviousEpsilonStates() const
{
    return previousEpsilonStates;
}

template<typename T>
const std::unordered_set<const NonDeterministicState<T> *> &NonDeterministicState<T>::getNextEpsilonStates() const
{
    return nextEpsilonStates;
}

template<typename T>
const std::unordered_set<const NonDeterministicState<T> *> *NonDeterministicState<T>::getPreviousState(const T &key) const
{
    if (previousStates.find(key) != previousStates.end())
    {
        return &(previousStates.at(key));
    }
    else
    {
        return nullptr;
    }
}

template<typename T>
const std::unordered_map<T, std::unordered_set<const NonDeterministicState<T> *>> &NonDeterministicState<T>::getPreviousStates() const
{
    return previousStates;
}

template<typename T>
void NonDeterministicState<T>::addPreviousState(const T &key, const NonDeterministicState<T>* previousState)
{
    previousStates[key].insert(previousState);
}

template<typename T>
void NonDeterministicState<T>::addPreviousEpsilonState(const NonDeterministicState<T>* previousState)
{
    previousEpsilonStates.insert(previousState);
}

template NonDeterministicState<char>;
