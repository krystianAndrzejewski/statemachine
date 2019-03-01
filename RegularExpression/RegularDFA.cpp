#include "RegularDFA.h"
#include <stack>
#include <algorithm>
#include <iterator>
#include <sstream>


RegularDFA::RegularDFA()
{
}


RegularDFA::~RegularDFA()
{
    for (auto state : states)
    {
        delete state;
    }
}

void RegularDFA::createFromInitialNfa(const NFA &nfa)
{
    states.clear();
    mapStates.clear();
    mapRDFAfromNFAStates.clear();
    finalStates.clear();
    std::stack<std::set<std::size_t>> setStack;
    std::unordered_map<char, std::set<std::size_t>> nextStates;
    auto initialStates = nfa.getStartStates();
    auto finalNFAStates = nfa.getFinalStates();
    initialState = 0;
    states.push_back(new DeterministicState(DeterministicState::Simple));
    mapRDFAfromNFAStates[initialStates] = states.back();
    mapNFAStates[states.back()] = initialStates;
    for (auto nextState : initialStates)
    {
        mapConnectedNFAStates[nextState].insert(initialStates);
    }
    mapStates[states.back()] = states.size() - 1;
    setStack.push(initialStates);
    std::set<std::size_t> intersect;
    std::set_intersection(
        initialStates.begin(),
        initialStates.end(),
        finalNFAStates.begin(),
        finalNFAStates.end(),
        std::inserter(intersect, intersect.begin()));
    if (!intersect.empty())
    {
        finalStates.insert(states.size() - 1);
    }
    do
    {
        auto actualSet = setStack.top();
        setStack.pop();
        nfa.returnMovePossibility(actualSet, nextStates);
        for (auto partOfStates : nextStates)
        {
            if (mapRDFAfromNFAStates.find(partOfStates.second) == mapRDFAfromNFAStates.end())
            {
                states.push_back(new DeterministicState(DeterministicState::Simple));
                mapRDFAfromNFAStates[partOfStates.second] = states.back();
                for (auto nextState : partOfStates.second)
                {
                    mapConnectedNFAStates[nextState].insert(partOfStates.second);
                }
                mapNFAStates[states.back()] = partOfStates.second;
                mapStates[states.back()] = states.size() - 1;
                setStack.push(partOfStates.second);
                intersect.clear();
                std::set_intersection(
                    partOfStates.second.begin(), 
                    partOfStates.second.end(), 
                    finalNFAStates.begin(), 
                    finalNFAStates.end(),
                    std::inserter(intersect, intersect.begin()));
                if (!intersect.empty())
                {
                    finalStates.insert(states.size() - 1);
                }
            }
            states[mapStates[mapRDFAfromNFAStates[actualSet]]]->addNextState(partOfStates.first, states[mapStates[mapRDFAfromNFAStates[partOfStates.second]]]);
        }
    } while (!setStack.empty());
    actualState = initialState;
}

void RegularDFA::createFromBackwardInitialNfa(const NFA &nfa)
{
    states.clear();
    mapStates.clear();
    mapRDFAfromNFAStates.clear();
    finalStates.clear();
    std::stack<std::set<std::size_t>> setStack;
    std::unordered_map<char, std::set<std::size_t>> previousStates;
    auto initialStates = nfa.getFinalStates();
    auto finalNFAStates = nfa.getStartStates();
    initialState = 0;
    states.push_back(new DeterministicState(DeterministicState::Simple));
    mapRDFAfromNFAStates[initialStates] = states.back();
    mapNFAStates[states.back()] = initialStates;
    for (auto nextState : initialStates)
    {
        mapConnectedNFAStates[nextState].insert(initialStates);
    }
    mapStates[states.back()] = states.size() - 1;
    setStack.push(initialStates);
    std::set<std::size_t> intersect;
    std::set_intersection(
        initialStates.begin(),
        initialStates.end(),
        finalNFAStates.begin(),
        finalNFAStates.end(),
        std::inserter(intersect, intersect.begin()));
    if (!intersect.empty())
    {
        finalStates.insert(states.size() - 1);
    }
    do
    {
        auto actualSet = setStack.top();
        setStack.pop();
        nfa.returnPreviousStates(actualSet, previousStates);
        for (auto partOfStates : previousStates)
        {
            if (mapRDFAfromNFAStates.find(partOfStates.second) == mapRDFAfromNFAStates.end())
            {
                states.push_back(new DeterministicState(DeterministicState::Simple));
                mapRDFAfromNFAStates[partOfStates.second] = states.back();
                for (auto nextState : partOfStates.second)
                {
                    mapConnectedNFAStates[nextState].insert(partOfStates.second);
                }
                mapNFAStates[states.back()] = partOfStates.second;
                mapStates[states.back()] = states.size() - 1;
                setStack.push(partOfStates.second);
                intersect.clear();
                std::set_intersection(
                    partOfStates.second.begin(),
                    partOfStates.second.end(),
                    finalNFAStates.begin(),
                    finalNFAStates.end(),
                    std::inserter(intersect, intersect.begin()));
                if (!intersect.empty())
                {
                    finalStates.insert(states.size() - 1);
                }
            }
            states[mapStates[mapRDFAfromNFAStates[actualSet]]]->addNextState(partOfStates.first, mapRDFAfromNFAStates[partOfStates.second]);
        }
    } while (!setStack.empty());
    actualState = initialState;
}

bool RegularDFA::returnNextState(char action, std::size_t &nextStateNumber)
{
    auto nextStates = states[actualState]->getNextStates();
    if (!nextStates.empty() && nextStates.find(action) != nextStates.end())
    {
        nextStateNumber = mapStates[*(nextStates[action].begin())];
        actualState = nextStateNumber;
        return true;
    }
    else
    {
        return false;
    }
}

void RegularDFA::returnMovePossibility(
    const std::size_t &number,
    std::unordered_map<char, std::size_t> &pNextStates) const
{
    pNextStates.clear();
    if (number < states.size())
    {
        for (auto partOfStates : states[number]->getNextStates())
        {
            for (auto state : partOfStates.second)
            {
                pNextStates[partOfStates.first] = mapStates.at(state);
            }
        }
    }
}

void RegularDFA::returnNFAStates(
    const std::size_t &number,
    std::set<std::size_t> &NFAstates) const
{
    NFAstates.clear();
    if (number < states.size())
    {
        NFAstates.insert(mapNFAStates.at(states.at(number)).begin(), mapNFAStates.at(states.at(number)).end());
    }
}


void RegularDFA::print(std::string &output) const
{
    std::size_t i = 0;
    std::stringstream stream;
    std::string intend = "    ";
    stream << "The simplified deterministic finite automata has " << states.size() << " states.\n\n";
    stream << "Initial state: " << initialState << "\n\n";
    stream << "Final state: ";
    for (auto finalState : finalStates)
    {
        stream << finalState << ", ";
    }
    stream << "\n\n";
    for (auto state : states)
    {
        if (state)
        {
            stream << i << ". state:\n";
            auto nextStates = state->getNextStates();
            auto previousStates = state->getPreviousStates();
            stream << intend << "Type: ";
            switch (state->returnType())
            {
            case State::Deterministic:
                stream << "Deterministic";
                break;
            case State::Entry:
                stream << "Entry";
                break;
            case State::Exit:
                stream << "Exit";
                break;
            case State::Priority:
                stream << "Priority";
                break;
            case State::NonDeterministic:
                stream << "NonDeterministic";
                break;
            }
            stream << "\n";
            stream << intend << "It consists of NFA's states: ";
            for (auto nfaState : mapNFAStates.at(state))
            {
                stream << nfaState << ", ";
            }
            stream << "\n";
            if (!nextStates.empty())
            {
                stream << intend << "Next states:\n";
                for (auto partOfNextState : nextStates)
                {
                    stream << intend << intend << "[ " << partOfNextState.first << " ] -> ";
                    for (auto nextState : partOfNextState.second)
                    {
                        stream << mapStates.at(nextState) << ", ";
                    }
                    stream << "\n";
                }
            }
            if (!previousStates.empty())
            {
                stream << intend << "Previous states:\n";
                for (auto partOfPreviousStates : previousStates)
                {
                    stream << intend << intend << "[ " << partOfPreviousStates.first << " ] -> ";
                    for (auto previousState : partOfPreviousStates.second)
                    {
                        stream << mapStates.at(previousState) << ", ";
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