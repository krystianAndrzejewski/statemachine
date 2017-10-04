#pragma once
#include "GroupDFA.h"  
#include "RegexAST.h"
#include "RegexMatch.h"

class Regex
{
public:
    Regex(const std::string &pPattern);
    ~Regex();

    void print(std::string &output);
    RegexMatch match(const std::string &input, std::size_t offset = 0);

private:

    RegexMatch useDFA(const std::string &input, std::size_t offset);
    RegexMatch useNFA(const std::string &input, std::size_t offset);
    bool processUsingRegularDfa(
        const std::string &input, 
        std::size_t offset, 
        std::size_t &lastInitialState, 
        std::size_t &lastFinalState);

    bool processUsingInvertedDfa(
        const std::string &input, 
        std::size_t offset, 
        std::size_t lastInitialState,
        std::size_t lastFinalState,
        std::vector<std::size_t> &invertedStates);

    bool processUsingGroupDfa(
        const std::vector<std::size_t> &invertedStates,
        std::vector<std::vector<std::size_t>> &groups);

    void createSubmatches(
        std::vector<std::vector<std::size_t>> &groups,
        std::vector < std::pair < std::size_t, std::size_t>> &submatches);

    void createMatches(
        const std::string &input,
        std::size_t offset,
        std::size_t lastInitialState,
        std::size_t lastFinalState,
        const std::vector<std::pair<std::size_t, std::size_t>> &submatches,
        std::vector<std::string> &matches);

    InitialDFA initAutomata;
    NFA nonDeterministicAutomata;
    RegularDFA automata;
    RegularDFA invertedAutomata;
    GroupDFA groupAutomata;
    RegexAST ast;

    bool success;
};

