#include "BaseRegexImpl.h"
#include <algorithm>



BaseRegexImpl::BaseRegexImpl()
{
}


BaseRegexImpl::~BaseRegexImpl()
{
}

bool BaseRegexImpl::processUsingGroupDfa(
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

bool BaseRegexImpl::processUsingInvertedDfa(
	const std::string &input,
	std::size_t offset,
	std::size_t lastInitialState,
	std::size_t lastFinalState,
	std::vector<std::size_t> &invertedStates)
{
	std::size_t startState = invertedAutomata.getStartState();
	auto finalStates = invertedAutomata.getFinalStates();
	invertedAutomata.returnToStartPosition();
	std::size_t actualState = invertedAutomata.getActualState();
	invertedStates.push_back(actualState);
	for (int i = (int)lastFinalState; i >= (int)lastInitialState; i--)
	{
		if (!invertedAutomata.returnNextState(input[i], actualState))
		{
			break;
		}
		invertedStates.push_back(actualState);
	}
	return finalStates.find(actualState) != finalStates.end();
}

bool BaseRegexImpl::processUsingRegularDfa(
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

void BaseRegexImpl::createSubmatches(
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

void BaseRegexImpl::createMatches(
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
