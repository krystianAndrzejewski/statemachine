#include "RegexMatch.h"
#include <algorithm>
#include <iterator>


RegexMatch::RegexMatch(const std::vector<std::string>& pResult, std::size_t pOffset) : offset(pOffset), empty(false)
{
    std::copy(pResult.begin(), pResult.end(), std::back_inserter(groups));
}

RegexMatch::RegexMatch() : empty(true)
{
}

RegexMatch::~RegexMatch()
{
}

bool RegexMatch::getGroup(std::string &pResult, std::size_t number)
{
    if (number < groups.size())
    {
        pResult = groups[number];
        return true;
    }
    return false;
}

void RegexMatch::getGroups(std::vector<std::string> &pResult)
{
    pResult.clear();
    std::copy(groups.begin(), groups.end(), std::back_inserter(pResult));
}