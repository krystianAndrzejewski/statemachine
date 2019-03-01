#pragma once
#include "GroupDFA.h"  

class BaseRegexImpl
{
protected:
	BaseRegexImpl();
	virtual ~BaseRegexImpl();

	virtual bool processUsingRegularDfa(
		const std::string &input,
		std::size_t offset,
		std::size_t &lastInitialState,
		std::size_t &lastFinalState);

	virtual  bool processUsingInvertedDfa(
		const std::string &input,
		std::size_t offset,
		std::size_t lastInitialState,
		std::size_t lastFinalState,
		std::vector<std::size_t> &invertedStates);

	virtual bool processUsingGroupDfa(
		const std::vector<std::size_t> &invertedStates,
		std::vector<std::vector<std::size_t>> &groups);

	virtual void createSubmatches(
		std::vector<std::vector<std::size_t>> &groups,
		std::vector < std::pair < std::size_t, std::size_t>> &submatches);

	virtual void createMatches(
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
};

