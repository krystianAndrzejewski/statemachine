#include "GroupState.h"


GroupState::GroupState()
{
}


GroupState::~GroupState()
{
}


bool GroupState::addNextState(std::set<std::size_t> key, GroupState *nextState, std::vector<std::size_t>& groups)
{
    if (nextState)
    {
        nextStates[key] = { nextState, std::vector<std::size_t>(groups.begin(), groups.end()) };
        nextState->addPreviousState(key, this, groups);
        return true;
    }
    return false;
}

const std::map<std::set<std::size_t>, std::pair<const GroupState *, std::vector<std::size_t>>> &GroupState::getNextStates() const
{
    return nextStates;
}
const std::map<std::set<std::size_t>, std::vector<std::pair<const GroupState *, std::vector<std::size_t>>>> &GroupState::getPreviousStates() const
{
    return previousStates;
}

void GroupState::addPreviousState(const std::set<std::size_t> key, const GroupState* previousState, std::vector<std::size_t>& groups)
{
    previousStates[key].push_back({ previousState, std::vector<std::size_t>(groups.begin(), groups.end()) });
}