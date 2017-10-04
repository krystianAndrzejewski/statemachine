#include "NFA.h"

#include <stack>
#include <sstream>
#include <algorithm>
#include <iterator>

NFA::NFA()
{
}


NFA::~NFA()
{
    for (auto state : states)
    {
        delete state;
    }
}


void NFA::createFromInitialDfa(const InitialDFA &dfa)
{
    states.clear();
    mapState.clear();
    finalStates.clear();
    initialStates.clear();
    actualStates.clear();
    std::vector<bool> visitedNodes(dfa.getSize(), false);
    std::stack<std::size_t> dfaDeterministicFinalStateStack;
    std::stack<std::size_t> dfaOtherStateStack;
    states.resize(dfa.getSize(), nullptr);
    State::StateType type;
    auto finalNumber = dfa.getFinalState();
    finalStates.insert(finalNumber);
    auto startNumber = dfa.getStartState(type);
    if (type == State::Deterministic)
    {
        dfaDeterministicFinalStateStack.push(startNumber);
        initialStates.insert(startNumber);
    }
    else
    {
        dfaOtherStateStack.push(startNumber);
    }
    std::unordered_map<char, std::pair<std::size_t, State::StateType>> lNextStates;
    std::size_t actualState;
    std::size_t mainState = states.size();
    do 
    {
        if (!dfaOtherStateStack.empty())
        {
            actualState = dfaOtherStateStack.top();
            dfaOtherStateStack.pop();
        }
        else
        {
            mainState = dfaDeterministicFinalStateStack.top();
            for (std::size_t i = 0; i < states.size(); i++)
            {
                visitedNodes[i] = states[i] ? true : false;
            }
            dfaDeterministicFinalStateStack.pop();
            actualState = mainState;
        }
        dfa.returnMovePossibility(actualState, lNextStates);
        for (auto nextState : lNextStates)
        {

            if (nextState.second.second == State::Deterministic || finalNumber == nextState.second.first)
            {
                if (visitedNodes[nextState.second.first] == false)
                {
                    visitedNodes[nextState.second.first] = true;
                    dfaDeterministicFinalStateStack.push(nextState.second.first);
                    states[nextState.second.first] = new State();
                    mapState[states[nextState.second.first]] = nextState.second.first;
                    if (mainState == states.size())
                    {
                        initialStates.insert(nextState.second.first);
                    }
                }
                if (mainState != states.size())
                {
                    std::unordered_map<char, std::pair<std::size_t, State::StateType>> tempNextStates;
                    dfa.returnMovePossibility(mainState, tempNextStates);
                    for (auto temp : tempNextStates)
                    {
                        states[mainState]->addNextState(temp.first, states[nextState.second.first]);
                    }
                }
            }
            else if(visitedNodes[nextState.second.first] == false)
            {
                dfaOtherStateStack.push(nextState.second.first);
                visitedNodes[nextState.second.first] = true;
            }
        }
    } while (!(dfaDeterministicFinalStateStack.empty() && dfaOtherStateStack.empty()));
    actualStates.insert(initialStates.begin(), initialStates.end());
}


bool NFA::returnNextState(char action, std::set<std::size_t > &nextStateNumber)
{
    nextStateNumber.clear();
    for (auto actualState : actualStates)
    {
        auto nextStates = states[actualState]->getNextStates();
        if (!nextStates.empty() && nextStates.find(action) != nextStates.end())
        {
            for (auto nextState : nextStates[action])
            {
                nextStateNumber.insert(mapState[nextState]);
            }
        }
    }
    if (nextStateNumber.empty())
    {
        return false;
    }
    else
    {
        actualStates.insert(nextStateNumber.begin(), nextStateNumber.end());
        return true;
    }
}

void NFA::returnMovePossibility(
    const std::set<std::size_t > &numbers,
    std::unordered_map<char, std::set<std::size_t>> &pNextStates) const
{
    pNextStates.clear();
    for (auto number : numbers)
    {
        if (number < states.size())
        {
            for (auto partOfStates : states[number]->getNextStates())
            {
                for (auto state : partOfStates.second)
                {
                    pNextStates[partOfStates.first].insert(mapState.at(state));
                }
            }
        }
    }
}

void NFA::returnPreviousStates(
    const std::set<std::size_t > &numbers,
    std::unordered_map<char, std::set<std::size_t>> &pPreviousStates) const
{
    pPreviousStates.clear();
    for (auto number : numbers)
    {
        if (number < states.size())
        {
            for (auto partOfStates : states[number]->getPreviousStates())
            {
                for (auto state : partOfStates.second)
                {
                    pPreviousStates[partOfStates.first].insert(mapState.at(state));
                }
            }
        }
    }
}

void NFA::print(std::string &output) const
{
    std::size_t i = 0;
    std::stringstream stream;
    std::string intend = "    ";
    std::size_t size = std::count_if(states.begin(), states.end(), [](State* value){ return value != nullptr; });
    stream << "The nondeterministic finite automata has " << size << " states.\n\n";
    stream << "Initial state: ";
    for (auto initialState : initialStates)
    {
        stream << initialState << ", ";
    }
    stream << "\n\n";
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
            if (!nextStates.empty())
            {
                stream << intend << "Next states:\n";
                for (auto partOfNextState : nextStates)
                {
                    stream << intend << intend << "[ " << partOfNextState.first << " ] -> ";
                    for (auto nextState : partOfNextState.second)
                    {
                        stream << mapState.at(nextState) << ", ";
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
                        stream << mapState.at(previousState) << ", ";
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