#include "GroupDFA.h"
#include "NFA.h"

#include <stack>
#include <queue>
#include <sstream>
#include <algorithm>
#include <iterator>


GroupDFA::GroupDFA()
{
}


GroupDFA::~GroupDFA()
{
    for (auto state : states)
    {
        delete state;
    }
}

void GroupDFA::createFromInitialDFA(const InitialDFA& initialDfa, RegularDFA &intvertedRegularDfa)
{
    NFA nfa;
    nfa.createFromInitialDfa(initialDfa);
    intvertedRegularDfa.createFromBackwardInitialNfa(nfa);
    finalState = initialDfa.getFinalState();
    std::size_t size = initialDfa.getSize();
    std::size_t i = 0;
    states.resize(size + 1);
    initialState = size;
    finalState = initialDfa.getFinalState();
    createGroupDfa(initialDfa, intvertedRegularDfa);
}

void GroupDFA::createGroupDfa(const InitialDFA& initialDfa, RegularDFA &regualarDfa)
{
    initialDfa.getGroups(groupsMap);
    std::size_t size = initialDfa.getSize();
    std::stack<std::size_t> statesStack;
    std::queue<std::size_t> mainQueue;
    State::StateType type;
    statesStack.push(initialDfa.getStartState(type));
    auto mapConnectedStates = regualarDfa.getMapConectedNFAStates();
    for (auto finalState : regualarDfa.getFinalStates())
    {
        std::set<std::size_t> connectedStates;
        regualarDfa.returnNFAStates(finalState, connectedStates);
        mapConnectedStates[size].insert(std::set<std::size_t>(connectedStates.begin(), connectedStates.end()));
    }
    std::size_t actualState = initialState;
    std::size_t processedState = size;
    states[processedState] = new GroupState();
    mapStates[states[processedState]] = actualState;
    std::stack<std::size_t> removingGroup;
    std::vector<std::size_t> group;
    std::vector<bool> flags;
    flags.resize(states.size());
    do
    {
        std::unordered_map<char, std::pair<std::size_t, State::StateType>> nextStates;
        if (!statesStack.empty())
        {
            actualState = statesStack.top();
            statesStack.pop();
            initialDfa.returnMovePossibility(actualState, nextStates);
            initialDfa.getType(actualState, type);
            while (!removingGroup.empty() && removingGroup.top() > statesStack.size())
            {
                removingGroup.pop();
                group.pop_back();
            }
            if ((type == State::Entry || type == State::Exit) && finalState != actualState)
            {
                removingGroup.push(statesStack.size());
            }
        }
        else
        {
            processedState = mainQueue.front();
            if (states[processedState] == nullptr)
            {
                states[processedState] = new GroupState();
                mapStates[states[processedState]] = processedState;
                initialState = processedState;
            }
            mainQueue.pop();
            actualState = processedState;
            initialDfa.returnMovePossibility(processedState, nextStates);
            for (auto nextState : nextStates)
            {
                statesStack.push(nextState.second.first);
            }
            group.clear();
            flags.clear();
            flags.resize(states.size());
            continue;
        }
        const auto &nextGroupsStates = states[processedState]->getNextStates();
        if (finalState == actualState && !flags[actualState])
        {
            if (states[actualState] == nullptr)
            {
                states[actualState] = new GroupState();
                mapStates[states[actualState]] = actualState;
                mainQueue.push(actualState);
            }
            for (auto connectedStates : mapConnectedStates[actualState])
            {
                if (nextGroupsStates.find(connectedStates) == nextGroupsStates.end())
                {
                    states[processedState]->addNextState(connectedStates, states[actualState], group);
                }
            }
        }
        else if (!flags[actualState])
        {
            switch (type)
            {
            case State::Priority:
                statesStack.push(nextStates['-'].first);
                statesStack.push(nextStates['+'].first);
                break;
            case State::Deterministic:
                if (states[actualState] == nullptr)
                {
                    states[actualState] = new GroupState();
                    mapStates[states[actualState]] = actualState;
                    mainQueue.push(actualState);
                }
                for (auto connectedStates : mapConnectedStates[actualState])
                {
                    if (nextGroupsStates.find(connectedStates) == nextGroupsStates.end())
                    {
                        states[processedState]->addNextState(connectedStates, states[actualState], group);
                    }
                }
                break;
            case State::Entry:
                group.push_back(groupsMap[actualState]);
                statesStack.push(nextStates[' '].first);
                break;
            case State::Exit:
                group.push_back(groupsMap[actualState] + groupsMap.size());
                statesStack.push(nextStates[' '].first);
                break;
            default:
                break;
            }
        }
        flags[actualState] = true;
    } while (!(mainQueue.empty() && statesStack.empty()));
}

void GroupDFA::print(std::string &output) const
{
    std::size_t i = 0;
    std::stringstream stream;
    std::string intend = "    ";
    std::size_t size = std::count_if(states.begin(), states.end(), [](GroupState* value){ return value != nullptr; });
    stream << "The group deterministic finite automata has " << size << " states.\n\n";
    stream << "Initial state: " << initialState << "\n\n";
    stream << "Final state: " << finalState << "\n\n";
    for (auto state : states)
    {
        if (state)
        {
            stream << i << ". state:\n";
            auto nextStates = state->getNextStates();
            auto previousStates = state->getPreviousStates();
            stream << "\n";
            if (!nextStates.empty())
            {
                stream << intend << "Next states:\n";
                for (auto partOfNextState : nextStates)
                {
                    stream << intend << intend << "[ ";
                    for (auto state : partOfNextState.first)
                    {
                        stream << state << ", ";
                    }
                    stream << "] -> { ";
                    stream << mapStates.at(partOfNextState.second.first) << " : ";
                    size = static_cast<std::size_t>(groupsMap.size() / 2) ;
                    for (auto group : partOfNextState.second.second)
                    {
                        if (size > group)
                        {
                            stream << "S" << group << ", ";
                        }
                        else
                        {
                            stream << "M" << group - size << ", ";
                        }
                    }
                    stream << "}\n";
                }
            }
            if (!previousStates.empty())
            {
                stream << intend << "Previous states:\n";
                for (auto partOfPreviousStates : previousStates)
                {
                    stream << intend << intend << "[ ";
                    for (auto state : partOfPreviousStates.first)
                    {
                        stream << state << ", ";
                    }
                    stream << "] -> ";
                    for (auto previousState : partOfPreviousStates.second)
                    {
                        stream << "{ " << mapStates.at(previousState.first) << " : ";
                        size = static_cast<std::size_t>(groupsMap.size() / 2);
                        for (auto group : previousState.second)
                        {
                            if (size > group)
                            {
                                stream << "S" << group << ", ";
                            }
                            else
                            {
                                stream << "M" << group - size << ", ";
                            }
                        }
                        stream << "}, ";
                    }
                    stream << "\n";
                }
            }
            stream << "\n";
        }
        i++;
    }
    stream << "\n";
    output += stream.str();
}

void GroupDFA::returnMovePossibility(
    const std::size_t &number,
    std::map<const std::set<std::size_t>, std::size_t> &pNextStates,
    std::vector<std::size_t> &groupNumbers) const
{
    pNextStates.clear();
    groupNumbers.clear();
    if (number < states.size())
    {
        for (auto state : states[number]->getNextStates())
        {
            pNextStates[state.first] = mapStates.at(state.second.first);
            std::copy(state.second.second.begin(), state.second.second.end(), std::back_inserter(groupNumbers));
        }
    }
}

bool GroupDFA::returnNextState(
    const std::set<std::size_t> &action, 
    std::size_t &nextStateNumber,
    std::vector<std::size_t> &groupNumbers)
{
    auto nextStates = states[actualState]->getNextStates();
    if (!nextStates.empty() && nextStates.find(action) != nextStates.end())
    {
        nextStateNumber = mapStates[nextStates[action].first];
        actualState = nextStateNumber;
        std::copy(nextStates[action].second.begin(), nextStates[action].second.end(), std::back_inserter(groupNumbers));
        return true;
    }
    else
    {
        return false;
    }
}