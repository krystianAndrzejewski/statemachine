#include "NonDeterministicFiniteAutomata.h"
#include "DeterministicFiniteAutomata.h"

#include <iterator>
#include <stack>


template<typename T>
NonDeterministicFiniteAutomata<T>::NonDeterministicFiniteAutomata(
    std::size_t size, std::size_t pInitialState, 
    const std::vector<std::size_t> &pFinalStates)
    : initialState(pInitialState), states(size)
{
    for (std::size_t i = 0; i < size; i++)
    {
        mapStates[&(states[i])] = i;
    }
    std::copy(pFinalStates.begin(), pFinalStates.end(), std::inserter(finalStates, finalStates.begin()));
}


template<typename T>
NonDeterministicFiniteAutomata<T>::~NonDeterministicFiniteAutomata()
{
}


template<typename T>
DeterministicFiniteAutomata<T> *NonDeterministicFiniteAutomata<T>::createDFA()
{
    findReachableStates();
    std::vector<std::vector<bool>> closureMap;
    std::vector<std::pair<std::size_t, const NonDeterministicState<T> *>> reachableMap;
    std::unordered_map<const NonDeterministicState<T> *, std::size_t> reachableMapNumber;
    std::unordered_map<std::vector<bool>, std::unordered_map<T, std::vector<bool>>> dfaStates;
    std::unordered_map<std::vector<bool>, std::pair<std::size_t, bool>> dfaNumberMap;
    std::stack<std::vector<bool>> statesStack;
    std::vector<bool> actualState;
    std::size_t i = 0;
    closureMap.resize(reachableStates.size());
    reachableMap.resize(reachableStates.size());
    for (auto reachableState : reachableStates)
    {
        reachableMap[i] = std::pair<std::size_t, const NonDeterministicState<T> * >(reachableState, &(states[reachableState]));
        reachableMapNumber[&(states[reachableState])] = i;
        closureMap[i].resize(reachableStates.size());
        i++;
    }
    i = 0;
    for (auto reachableState : reachableStates)
    {
        closureMap[i][i] = true;
        for (auto epsilonState : states[reachableState].getNextEpsilonStates())
        {
            closureMap[i][reachableMapNumber[epsilonState]] = true;
        }
        i++;
    }
    actualState.resize(reachableMap.size());
    actualState[reachableMap[initialState].first] = true;
    statesStack.push(actualState);
    dfaNumberMap[actualState] = std::pair<std::size_t, bool>(0, finalStates.find(initialState) != finalStates.end());
    actualState.resize(reachableMap.size());
    std::size_t k = 0;
    do
    {
        auto processedState = statesStack.top();
        std::unordered_map<T, std::unordered_set<const NonDeterministicState<T> *>> allNextStates;
        for (i = 0; i < processedState.size(); i++)
        {
            if (processedState[i])
            {
                for (auto nextStates : reachableMap[i].second->getNextStates())
                {
                    allNextStates[nextStates.first].insert(nextStates.second.begin(), nextStates.second.end());
                }
                
            }
        }
        statesStack.pop();
        for (auto nextStates : allNextStates)
        {
            bool isFinal = false;
            std::fill(actualState.begin(), actualState.end(), false);
            for (auto nextState : nextStates.second)
            {
                for (i = 0; i < processedState.size(); i++)
                {
                    isFinal = isFinal || 
                        (closureMap[reachableMapNumber[nextState]][i] && 
                        finalStates.find(reachableMap[i].first) != finalStates.end());
                    actualState[i] = actualState[i] || closureMap[reachableMapNumber[nextState]][i];
                }
            }
            dfaStates[processedState][nextStates.first] = actualState;
            if (dfaNumberMap.find(actualState) == dfaNumberMap.end())
            {
                k++;
                dfaNumberMap[actualState] = std::pair<std::size_t, bool>(k, isFinal);
                statesStack.push(actualState);
            }
        }
    } while (!statesStack.empty());
    std::vector<std::size_t> pFinalStates;
    std::fill(actualState.begin(), actualState.end(), false);
    actualState[reachableMap[initialState].first] = true;
    for (auto dfaNumber : dfaNumberMap)
    {
        if (dfaNumber.second.second)
        {
            pFinalStates.push_back(dfaNumber.second.first);
        }
    }
    DeterministicFiniteAutomata<T> &dfa = *(new DeterministicFiniteAutomata<T>(dfaStates.size(), 0, pFinalStates));
    std::vector<std::tuple<std::size_t, std::size_t, T>> links;
    for (auto state : dfaStates)
    {
        auto nextStates = state.second;
        for (auto nextState : nextStates)
        {
            links.push_back(
                std::tuple<std::size_t, std::size_t, T>(
                dfaNumberMap[state.first].first,
                dfaNumberMap[nextState.second].first,
                nextState.first)
                );
        }
    }
    for (auto link : links)
    {
        dfa.linkStates(std::get<0>(link), std::get<1>(link), std::get<2>(link));
    }
    return &dfa;
}

template<typename T>
void NonDeterministicFiniteAutomata<T>::linkStates(std::size_t precedessor, std::size_t consequent)
{
    states[precedessor].addNextEpsilonState(&(states[consequent]));
}

template<typename T>
void NonDeterministicFiniteAutomata<T>::linkStates(std::size_t precedessor, std::size_t consequent, const T condition)
{
    states[precedessor].addNextState(condition, &(states[consequent]));
}

template<typename T>
void NonDeterministicFiniteAutomata<T>::findReachableStates()
{
    std::unordered_set<std::size_t> newStates;
    reachableStates.insert(initialState);
    newStates.insert(initialState);
    std::unordered_set<std::size_t> tmp;
    do
    {
        tmp.clear();
        for (auto state : newStates)
        {
            for (auto consequents : states[state].getNextStates())
            {
                for (auto consequent : consequents.second)
                {
                    if (reachableStates.find(mapStates[consequent]) == reachableStates.end())
                    {
                        tmp.insert(mapStates[consequent]);
                        reachableStates.insert(mapStates[consequent]);
                    }
                }
            }
        }
        newStates = std::move(tmp);
    } while (!newStates.empty());
}

template NonDeterministicFiniteAutomata<char>;