#include "DeterministicFiniteAutomata.h"

#include <algorithm>
#include <iterator> 

template<typename T>
DeterministicFiniteAutomata<T>::DeterministicFiniteAutomata(
    std::size_t size, 
    std::size_t pInitialState, 
    const std::vector<std::size_t> &pFinalStates)
    : initialState(pInitialState), actualState(pInitialState), states(size)
{
    for (std::size_t i = 0; i < size; i++)
    {
        mapStates[&(states[i])] = i;
    }
    std::copy(pFinalStates.begin(), pFinalStates.end(), std::inserter(finalStates, finalStates.begin()));
}


template<typename T>
DeterministicFiniteAutomata<T>::~DeterministicFiniteAutomata()
{

}

template<typename T>
bool DeterministicFiniteAutomata<T>::linkStates(std::size_t precedessor, std::size_t consequent, const T condition)
{
    return states[precedessor].addNextState(condition, &(states[consequent]));
}

template<typename T>
void difference(std::unordered_set<T> &first, std::unordered_set<T> &second, std::unordered_set<T> &result)
{

    std::copy_if(
        first.begin(),
        first.end(),
        std::inserter(result, result.end()),
        [&second](const T &key)
    {
        return second.find(key) == second.end();
    }
    );
}

template<typename T>
void intersect(std::unordered_set<T> &first, std::unordered_set<T> &second, std::unordered_set<T> &result)
{
    std::copy_if(
        first.begin(),
        first.end(),
        std::inserter(result, result.end()),
        [&second](const T &key)
    {
        return second.find(key) != second.end();
    }
    );
}

template<typename T>
bool DeterministicFiniteAutomata<T>::returnNextState(const T action, std::size_t &nextStateNumber)
{
    auto nextState = states[actualState].getNextState(action);
    if (nextState)
    {
        nextStateNumber = mapStates[nextState];
        actualState = nextStateNumber;
        return true;
    }
    else
    {
        return false;
    }
}

template<typename T>
void DeterministicFiniteAutomata<T>::simplifyDFA()
{
    findReachableStates();
    std::vector<std::unordered_set<std::size_t>> statesGroups;
    HopcroftAlgorithm(statesGroups);
    createSimplifiedStateMachine(statesGroups);
}

template<typename T>
void DeterministicFiniteAutomata<T>::findReachableStates()
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
            for (auto consequent : states[state].getNextStates())
            {
                if (reachableStates.find(mapStates[consequent.second]) == reachableStates.end())
                {
                    tmp.insert(mapStates[consequent.second]);
                    reachableStates.insert(mapStates[consequent.second]);
                }
            }
        }
        newStates = std::move(tmp);
    } while (!newStates.empty());
}

template <typename T>
void DeterministicFiniteAutomata<T>::HopcroftAlgorithm(std::vector<std::unordered_set<std::size_t>> &statesGroups)
{
    statesGroups.resize(2);
    for (auto finalState : finalStates)
    {
        statesGroups[0].insert(finalState);
    }
    difference(reachableStates, statesGroups[0], statesGroups[1]);
    std::unordered_set<std::size_t> processedSets;
    if (statesGroups[0].size() > statesGroups[1].size())
    {
        processedSets.insert(0);
    }
    else
    {
        processedSets.insert(1);
    }
    while (!processedSets.empty())
    {
        std::unordered_set<std::size_t> processedSet;

        std::copy(statesGroups[*(processedSets.begin())].begin(),
            statesGroups[*(processedSets.begin())].end(),
            std::inserter(processedSet, processedSet.end()));

        processedSets.erase(*(processedSets.begin()));
        std::unordered_map<T, std::unordered_set<std::size_t>> newSets;

        checkPreviousStateForStateSet(processedSet, newSets);

        divideStatesGroups(statesGroups, newSets, processedSets);
    }
}


template <typename T>
void DeterministicFiniteAutomata<T>::checkPreviousStateForStateSet(
    std::unordered_set<std::size_t> &actualStates,
    std::unordered_map<T, std::unordered_set<std::size_t>> &previousStatesDictionary)
{
    for (auto state : actualStates)
    {
        for (auto previousStates : states[state].getPreviousStates())
        {
            for (auto previousState : previousStates.second)
            {
                previousStatesDictionary[previousStates.first].insert(mapStates[previousState]);
            }
        }
    }
}

template <typename T>
void DeterministicFiniteAutomata<T>::divideStatesGroups(
    std::vector<std::unordered_set<std::size_t>> &statesGroups,
    std::unordered_map<T, std::unordered_set<std::size_t>> &previousStatesDictionary,
    std::unordered_set<std::size_t> &processedSets)
{
    for (auto newSet : previousStatesDictionary)
    {
        auto actualSize = statesGroups.size();
        for (std::size_t i = 0; i < actualSize; i++)
        {
            std::unordered_set<std::size_t> common, diff;
            intersect(statesGroups[i], newSet.second, common);
            difference(statesGroups[i], newSet.second, diff);
            if (!common.empty() && !diff.empty())
            {
                statesGroups[i].clear();
                statesGroups.push_back(std::unordered_set<std::size_t>());
                statesGroups[i].clear();
                statesGroups[i].insert(common.begin(), common.end());
                statesGroups.back().insert(diff.begin(), diff.end());
                if (processedSets.find(i) != processedSets.end())
                {
                    processedSets.insert(processedSets.size() - 1);
                }
                else
                {
                    processedSets.insert(diff.size() > common.size() ? i : processedSets.size() - 1);
                }
            }
        }
    }
}

template<typename T>
void DeterministicFiniteAutomata<T>::createSimplifiedStateMachine(std::vector<std::unordered_set<std::size_t>> &statesGroups)
{
    std::unordered_map<std::size_t, std::size_t> newStatesMap;
    for (std::size_t i = 0; i < statesGroups.size(); i++)
    {
        for (auto state : statesGroups[i])
        {
            newStatesMap[state] = i;
        }
    }

    std::vector<std::tuple<std::size_t, std::size_t, T>> links;
    for (auto state : reachableStates)
    {
        auto nextStates = states[state].getNextStates();
        for (auto nextState : nextStates)
        {
            links.push_back(
                std::tuple<std::size_t, std::size_t, T>(
                    newStatesMap[state], 
                    newStatesMap[mapStates[nextState.second]], 
                    nextState.first)
                );
        }
    }
    states.clear();
    mapStates.clear();
    reachableStates.clear();
    states.resize(statesGroups.size());
    for (std::size_t i = 0; i < states.size(); i++)
    {
        mapStates[&(states[i])] = i;
    }

    for (auto link : links)
    {
        linkStates(std::get<1>(link), std::get<0>(link), std::get<2>(link));
    }
}

template DeterministicFiniteAutomata<char>;