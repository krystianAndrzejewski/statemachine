#include "Regex.h"
#include "RegexAST.h"

#include <sstream>


Regex::Regex(const std::string &pPattern) : ast(pPattern)
{
    success = ast.createAutomatas(initAutomata, nonDeterministicAutomata, automata, invertedAutomata, groupAutomata);
}


Regex::~Regex()
{
}


void Regex::print(std::string &output)
{
    ast.print(output);
    initAutomata.print(output);
    if (ast.isGreedy())
    {
        nonDeterministicAutomata.print(output);
        automata.print(output);
        invertedAutomata.print(output);
        groupAutomata.print(output);
    }
}

RegexMatch Regex::match(const std::string &input, std::size_t offset)
{
    if (ast.isGreedy())
    {
        return useDFA(input, offset);
    }
    else
    {
        return useNFA(input, offset);
    }
}

RegexMatch Regex::useDFA(const std::string &input, std::size_t offset)
{
    std::size_t lastFinalState = 0;
    std::size_t lastInitialState = 0;
    std::vector<std::size_t> invertedStates;
    std::vector<std::vector<std::size_t>> groups;
    std::vector<std::pair < std::size_t, std::size_t>> submatches;
    std::vector<std::string> matches;

    bool success = processUsingRegularDfa(input, offset, lastInitialState, lastFinalState) &&
        processUsingInvertedDfa(input, offset, lastInitialState, lastFinalState, invertedStates) &&
        processUsingGroupDfa(invertedStates, groups);

    if (success)
    {
        createSubmatches(groups, submatches);
        createMatches(input, offset, lastInitialState, lastFinalState, submatches, matches);
        return RegexMatch(matches, lastInitialState);
    }
    else
    {
        return RegexMatch();
    }
}


RegexMatch Regex::useNFA(const std::string &input, std::size_t offset)
{
    State::StateType type;
    auto startState = initAutomata.getStartState(type);
    auto finalState = initAutomata.getFinalState();
    bool wasFinalState = false;
    std::map<std::size_t, std::size_t> groupMap;
    initAutomata.getGroups(groupMap);
	initAutomata.returnToStartPosition();
    std::size_t actualState = initAutomata.getActualState(type);
    std::vector<std::stack<std::size_t>> inputStack;
    std::vector<std::vector<std::size_t>> groups;
    std::size_t lastInitialState = input.size();
    std::size_t lastFinalState = input.size();
    inputStack.push_back(std::stack<std::size_t>());
    groups.push_back(std::vector<std::size_t>());
    inputStack.back().push(actualState);
    for (std::size_t i = offset; i < input.size() && !wasFinalState; i++)
    {
        lastInitialState = i;
        while (!inputStack.empty())
        {
            inputStack.back().pop();
            actualState = initAutomata.getActualState(type);
            if (finalState == actualState)
            {
                std::sort(groups.back().begin(), groups.back().end());
                wasFinalState = true;
                lastFinalState = i - 1;
                break;
            }
            std::unordered_map<char, std::pair<std::size_t, State::StateType>> nextStates;
            switch (type)
            {
            case State::Deterministic:
                if (initAutomata.returnNextState(input[i], actualState))
                {
                    i++;
                    inputStack.push_back(std::stack<std::size_t>());
                    std::sort(groups.back().begin(), groups.back().end());
                    groups.push_back(std::vector<std::size_t>());
                    inputStack.back().push(actualState);
                }
                else
                {
                    while (!inputStack.empty() && inputStack.back().empty())
                    {
                        inputStack.pop_back();
                        groups.pop_back();
                        i--;
                    }
                    if (!inputStack.empty())
                    {
                        initAutomata.setState(inputStack.back().top());
                    }
                }
                break;
            case State::Entry:
                groups.back().push_back(groupMap[actualState]);
                initAutomata.returnNextState(' ', actualState);
                inputStack.back().push(actualState);
                break;
            case State::Exit:
                groups.back().push_back(groupMap[actualState]);
                initAutomata.returnNextState(' ', actualState);
                inputStack.back().push(actualState);
                break;
            case State::Priority:
                initAutomata.returnMovePossibility(actualState, nextStates);
                inputStack.back().push(nextStates['-'].first);
                initAutomata.returnNextState('+', actualState);
                inputStack.back().push(actualState);
                break;
            default:
                break;
            }
        }
        if (!wasFinalState)
        {
            initAutomata.returnToStartPosition();
            actualState = initAutomata.getActualState(type);
            inputStack.push_back(std::stack<std::size_t>());
            groups.push_back(std::vector<std::size_t>());
            inputStack.back().push(actualState);
            i = lastInitialState;
        }
    }
    if (success)
    {
        std::vector < std::pair < std::size_t, std::size_t>> submatches;
        std::vector<std::string> matches;
        createSubmatches(groups, submatches);
        createMatches(input, offset, lastInitialState, lastFinalState, submatches, matches);
        return RegexMatch(matches, lastInitialState);
    }
    else
    {
        return RegexMatch();
    }
}