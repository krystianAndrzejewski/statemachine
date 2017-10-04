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

bool Regex::processUsingGroupDfa(
    const std::vector<std::size_t> &invertedStates,
    std::vector<std::vector<std::size_t>> &groups)
{
    groupAutomata.returnToStartPosition();
    std::size_t actualState = groupAutomata.getActualState();
    for (auto it = invertedStates.rbegin(); it != invertedStates.rend(); it++)
    {
        std::set<std::size_t> nfaStates;
        std::vector<std::size_t> tmp;
        invertedAutomata.returnNFAStates(*it, nfaStates);
        groups.push_back({});
        if (!groupAutomata.returnNextState(nfaStates, actualState, groups.back()))
        {
            return false;
        }
        std::sort(groups.back().begin(), groups.back().end());
    }
    return true;
}

bool Regex::processUsingInvertedDfa(
    const std::string &input,
    std::size_t offset,
    std::size_t lastInitialState,
    std::size_t lastFinalState,
    std::vector<std::size_t> &invertedStates)
{
    std::size_t startState = invertedAutomata.getStartState();
    auto finalStates = invertedAutomata.getFinalStates();
    std::size_t actualState = invertedAutomata.getActualState();
    invertedStates.push_back(actualState);
    for (std::size_t i = lastFinalState; i >= lastInitialState; i--)
    {
        if (!invertedAutomata.returnNextState(input[i], actualState))
        {
            break;
        }
        invertedStates.push_back(actualState);
    }
    return finalStates.find(actualState) != finalStates.end();
}

bool Regex::processUsingRegularDfa(
    const std::string &input, 
    std::size_t offset, 
    std::size_t &lastInitialState, 
    std::size_t &lastFinalState)
{
    std::vector<bool> checkedPosition;
    checkedPosition.resize(input.size());
    std::size_t startState = automata.getStartState();
    auto finalStates = automata.getFinalStates();
    bool wasFinalState = false;
    for (std::size_t i = offset; i < input.size(); i++)
    {
        if (checkedPosition[i] == false)
        {
            automata.returnToStartPosition();
            std::size_t actualState = automata.getActualState();
            if (finalStates.find(actualState) != finalStates.end())
            {
                lastFinalState = i;
            }
            checkedPosition[i] = true;
            for (std::size_t j = i; j < input.size(); j++)
            {
                if (!automata.returnNextState(input[j], actualState))
                {
                    break;
                }
                if (finalStates.find(actualState) != finalStates.end())
                {
                    lastFinalState = j;
                    wasFinalState = true;
                }
                if (actualState == startState)
                {
                    checkedPosition[j] = true;
                }
            }
            if (wasFinalState)
            {
                lastInitialState = i;
                break;
            }
        }
    }
    return wasFinalState;
}


RegexMatch Regex::useNFA(const std::string &input, std::size_t offset)
{
    State::StateType type;
    auto startState = initAutomata.getStartState(type);
    auto finalState = initAutomata.getFinalState();
    bool wasFinalState = false;
    std::map<std::size_t, std::size_t> groupMap;
    initAutomata.getGroups(groupMap);
    std::size_t actualState = initAutomata.getActualState(type);
    std::vector<std::stack<char>> inputStack;
    std::vector<std::vector<std::size_t>> groups;
    std::size_t lastInitialState = input.size();
    std::size_t lastFinalState = input.size();
    inputStack.push_back(std::stack<char>());
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
                    inputStack.push_back(std::stack<char>());
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
                groups.back().push_back(groupMap[actualState] + groupMap.size() / 2);
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
            inputStack.push_back(std::stack<char>());
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

void Regex::createSubmatches(
    std::vector<std::vector<std::size_t>> &groups,
    std::vector < std::pair < std::size_t, std::size_t>> &submatches)
{
    submatches.clear();
    submatches.resize(initAutomata.getNumberOfGroups());
    for (auto& submatch : submatches)
    {
        submatch.first = groups.size();
        submatch.second = groups.size();
    }
    std::size_t i = groups.size() - 1;
    for (auto it = groups.rbegin(); it != groups.rend(); it++, i--)
    {
        for (auto groupNumber : *it)
        {
            if (groupNumber < submatches.size())
            {
                if (submatches[groupNumber].second != groups.size() && submatches[groupNumber].first == groups.size())
                {
                    submatches[groupNumber].first = i;
                }
            }
            else
            {
                if (submatches[groupNumber - initAutomata.getNumberOfGroups()].second == groups.size())
                {
                    submatches[groupNumber - initAutomata.getNumberOfGroups()].second = i;
                }
            }
        }
    }
}

void Regex::createMatches(
    const std::string &input,
    std::size_t offset,
    std::size_t lastInitialState,
    std::size_t lastFinalState,
    const std::vector<std::pair<std::size_t, std::size_t>> &submatches,
    std::vector<std::string> &matches)
{
    matches.resize(submatches.size() + 1);
    matches[0] = input.substr(lastInitialState, lastFinalState - lastInitialState + 1);
    for (std::size_t i = 0; i < submatches.size(); i++)
    {
        if (submatches[i].first < submatches[i].second)
        {
            matches[i + 1] = input.substr(lastInitialState + submatches[i].first, submatches[i].second - submatches[i].first);
        }
    }
}