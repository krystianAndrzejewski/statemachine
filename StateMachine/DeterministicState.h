#pragma once
#include <unordered_map>
#include <unordered_set>

template<typename T>
class DeterministicState
{
public:
    DeterministicState();
    ~DeterministicState();
    bool addNextState(const T &key, DeterministicState<T> *nextState);

    const DeterministicState *getNextState(const T &key);
    const std::unordered_map<T, const DeterministicState<T> *> &getNextStates();

    const std::unordered_set<const DeterministicState<T> *> *getPreviousState(const T &key);
    const std::unordered_map<T, std::unordered_set<const DeterministicState<T> *>> &getPreviousStates();

private:
    void addPreviousState(const T &key, const DeterministicState<T>* nextState);

    std::unordered_map<T, const DeterministicState<T> *> nextStates;
    std::unordered_map<T, std::unordered_set<const DeterministicState<T> *>> previousStates;

};

