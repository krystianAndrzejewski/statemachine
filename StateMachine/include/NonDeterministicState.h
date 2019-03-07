#pragma once
#include <unordered_map>
#include <unordered_set>

template<typename T>
class NonDeterministicState
{
public:
    NonDeterministicState();
    ~NonDeterministicState();
    void addNextState(const T &key, NonDeterministicState<T> *nextState);
    void addNextEpsilonState(NonDeterministicState<T> *nextState);

    const std::unordered_set<const NonDeterministicState<T> *> *getNextState(const T &key) const;
    const std::unordered_map<T, std::unordered_set<const NonDeterministicState<T> *>> &getNextStates() const;

    const std::unordered_set<const NonDeterministicState<T> *> *getPreviousState(const T &key) const;
    const std::unordered_map<T, std::unordered_set<const NonDeterministicState<T> *>> &getPreviousStates() const;

    const std::unordered_set<const NonDeterministicState<T> *> &getPreviousEpsilonStates() const;
    const std::unordered_set<const NonDeterministicState<T> *> &getNextEpsilonStates() const;

private:
    void addPreviousState(const T &key, const NonDeterministicState<T>* nextState);
    void addPreviousEpsilonState(const NonDeterministicState<T>* previousState);

    std::unordered_map<T, std::unordered_set<const NonDeterministicState<T> *>> nextStates;
    std::unordered_map<T, std::unordered_set<const NonDeterministicState<T> *>> previousStates;

    std::unordered_set<const NonDeterministicState<T> *> nextEpsilonStates;
    std::unordered_set<const NonDeterministicState<T> *> previousEpsilonStates;

};

