#include "InitialDFA.h"

#include <sstream>
#include <stack>

InitialDFA::InitialDFA() : finalState(0), initialState(0), actualState(0)
{
    createNewState(DeterministicState::Simple);
}

InitialDFA::~InitialDFA()
{
    for (auto state : states)
    {
        delete state;
    }
}

void InitialDFA::createNewState(DeterministicState::DetermisitcStateType type)
{
    states.push_back(new DeterministicState(type));
    mapStates[states.back()] = states.size() - 1;
}

void InitialDFA::addNewState(State *state)
{
    states.push_back(state);
    mapStates[state] = states.size() - 1;
}

void InitialDFA::concatenation(InitialDFA& consequent)
{
    auto startConsequentState = consequent.states[consequent.initialState];
    auto lastActualState = states[finalState];
    for (auto state : consequent.states)
    {
        if (startConsequentState != state)
        {
            addNewState(state);
            if (state == consequent.states[consequent.finalState])
            {
                finalState = states.size() - 1;
            }
        }
    }
    static_cast<DeterministicState*>(lastActualState)->changeTypeState(
        static_cast<DeterministicState::DetermisitcStateType>(startConsequentState->returnType()));
    if (startConsequentState->returnType() == State::Entry || startConsequentState->returnType() == State::Exit)
    {
        auto connectedState = static_cast<DeterministicState*>(startConsequentState)->getConnectedState();
        static_cast<DeterministicState*>(startConsequentState)->divideGroupState(
            static_cast<DeterministicState*>(states[mapStates[connectedState]]));
        static_cast<DeterministicState*>(lastActualState)->connectGroupState(
            static_cast<DeterministicState*>(states[mapStates[connectedState]]));
    }
    startConsequentState->swapInNextStates(lastActualState);
    startConsequentState->swapInPreviousStates(lastActualState);
    delete startConsequentState;
    consequent.states.clear();
    consequent.mapStates.clear();
}

void InitialDFA::transition(const std::vector<char> &characterSet)
{
    createNewState(DeterministicState::Simple);
    for (auto character : characterSet)
    {
        states[finalState]->addNextState(character, states.back());
    }
    finalState = states.size() - 1;
}

void InitialDFA::alternative(InitialDFA& other)
{
    auto lastOtherState = other.states[other.finalState];
    auto lastActualState = states[finalState];
    lastActualState->swapInPreviousStates(lastOtherState);
    static_cast<DeterministicState*>(lastActualState)->changeTypeState(DeterministicState::Priority);
    lastActualState->addNextState('+', states[initialState]);
    lastActualState->addNextState('-', other.states[other.initialState]);
    for (auto state : other.states)
    {
        addNewState(state);
    }
    finalState = mapStates[lastOtherState];
    initialState = mapStates[lastActualState];
    actualState = initialState;
    other.states.clear();
    other.mapStates.clear();
}

void InitialDFA::greedyRepeation()
{
    createNewState(DeterministicState::Priority);
    auto startState = states.back();
    createNewState(DeterministicState::Simple);
    auto lastState = states.back();
    static_cast<DeterministicState*>(states[finalState])->changeTypeState(DeterministicState::Priority);
    states[finalState]->addNextState('+', states[initialState]);
    states[finalState]->addNextState('-', lastState);
    startState->addNextState('+', states[initialState]);
    startState->addNextState('-', lastState);
    finalState = mapStates[lastState];
    initialState = mapStates[startState];
    actualState = initialState;
}

void InitialDFA::lazyRepeation()
{
    createNewState(DeterministicState::Priority);
    auto startState = states.back();
    createNewState(DeterministicState::Simple);
    auto lastState = states.back();
    static_cast<DeterministicState*>(states[finalState])->changeTypeState(DeterministicState::Priority);
    states[finalState]->addNextState('-', states[initialState]);
    states[finalState]->addNextState('+', lastState);
    startState->addNextState('-', states[initialState]);
    startState->addNextState('+', lastState);
    finalState = mapStates[lastState];
    initialState = mapStates[startState];
    actualState = initialState;
}

void InitialDFA::greedyRepeationPlus()
{
    createNewState(DeterministicState::Simple);
    auto lastState = states.back();
    static_cast<DeterministicState*>(states[finalState])->changeTypeState(DeterministicState::Priority);
    states[finalState]->addNextState('+', states[initialState]);
    states[finalState]->addNextState('-', lastState);
    finalState = mapStates[lastState];
}

void InitialDFA::lazyRepeationPlus()
{
    createNewState(DeterministicState::Simple);
    auto lastState = states.back();
    static_cast<DeterministicState*>(states[finalState])->changeTypeState(DeterministicState::Priority);
    states[finalState]->addNextState('-', states[initialState]);
    states[finalState]->addNextState('+', lastState);
    finalState = mapStates[lastState];
}

void InitialDFA::zeroOrOne()
{
    createNewState(DeterministicState::Priority);
    auto startState = states.back();
    static_cast<DeterministicState*>(states[finalState])->changeTypeState(DeterministicState::Priority);
    startState->addNextState('+', states[mapStates[startState]]);
    startState->addNextState('-', states[finalState]);
    initialState = mapStates[startState];
    actualState = initialState;
}

void InitialDFA::group()
{
    createNewState(DeterministicState::Entry);
    auto startState = states.back();
    createNewState(DeterministicState::Simple);
    auto lastState = states.back();
    static_cast<DeterministicState*>(states[finalState])->changeTypeState(DeterministicState::Exit);
    static_cast<DeterministicState*>(states[finalState])->connectGroupState(static_cast<DeterministicState*>(startState));
    startState->addNextState(states[initialState]);
    states[finalState]->addNextState(lastState);
    finalState = mapStates[lastState];
    initialState = mapStates[startState];
    actualState = initialState;
}

bool InitialDFA::returnNextState(char action, std::size_t &nextStateNumber)
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

bool InitialDFA::setState(std::size_t state)
{
    if (state < states.size())
    {
        actualState = state;
        return true;
    }
    return false;
}

void InitialDFA::returnMovePossibility(
    const std::size_t &number, 
    std::unordered_map<char, std::pair<std::size_t, State::StateType>> &pNextStates) const
{
    pNextStates.clear();
    if (number < states.size())
    {
        for (auto partOfStates : states[number]->getNextStates())
        {
            for (auto state : partOfStates.second)
            {
                pNextStates[partOfStates.first] = std::pair<std::size_t, State::StateType>(mapStates.at(state), state->returnType());
            }
        }
    }

}

void InitialDFA::getGroups(std::map<std::size_t, std::size_t> &groups) const
{
    groups.clear();
    std::stack<std::size_t> statesStack;
    std::vector<bool> flags;
	std::vector<std::size_t> exitStates;
    State::StateType type;
    statesStack.push(initialState);
    flags.resize(states.size());
    std::size_t actualGroup = 0;
    do
    {
        auto processedState = statesStack.top();
        statesStack.pop();
        const auto &nextGroupsStates = states[processedState]->getNextStates();
        if (!flags[processedState] && getType(processedState, type))
        {
            switch (type)
            {
            case State::Entry:
				exitStates.push_back(mapStates.at(static_cast<DeterministicState*>(states[processedState])->getConnectedState()));
                groups[mapStates.at(static_cast<DeterministicState*>(states[processedState])->getConnectedState())] = actualGroup;
                groups[processedState] = actualGroup;
                actualGroup++;
                statesStack.push(mapStates.at(*(nextGroupsStates.at(' ').begin())));
                break;
            case State::Exit:
                statesStack.push(mapStates.at(*(nextGroupsStates.at(' ').begin())));
                break;
            case State::Priority:
                statesStack.push(mapStates.at(*(nextGroupsStates.at('-').begin())));
                statesStack.push(mapStates.at(*(nextGroupsStates.at('+').begin())));
                break;
            case State::Deterministic:
                for (auto nextState : nextGroupsStates)
                {
                    statesStack.push(mapStates.at(*(nextState.second.begin())));
                }
                break;
            default:
                break;
            }
        }
        flags[processedState] = true;
    } while (!statesStack.empty());

	for (auto stateID : exitStates)
	{
		groups[stateID] = exitStates.size() + groups[stateID];
	}
}

std::size_t InitialDFA::getNumberOfGroups() const
{
    std::size_t result = 0;
    for (auto state : states)
    {
        if (state->returnType() == State::Entry)
        {
            result++;
        }
    }
    return result;
}

bool InitialDFA::getType(std::size_t number, State::StateType& type) const
{
    if (number >= 0 && number < states.size() && states.at(number))
    {
        type = states.at(number)->returnType();
        return true;
    }
    return false;
}

void InitialDFA::print(std::string &output) const
{
    std::size_t i = 0;
    std::stringstream stream;
    std::string intend = "    ";
    stream << "The initial deterministic finite automata has " << states.size() << " states.\n\n";
    stream << "Initial state: " << initialState << "\n\n";
    stream << "Final state: " << finalState << "\n\n";
    for (auto state : states)
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
        i++;
    }
    stream << "\n";
    output += stream.str();
}