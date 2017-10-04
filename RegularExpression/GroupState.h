#pragma once
#include "State.h"
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

class GroupState
{
public:
    GroupState();
    ~GroupState();
    bool addNextState(std::set<std::size_t> key, GroupState *nextState, std::vector<std::size_t>& groups);

    const std::map<std::set<std::size_t>, std::pair<const GroupState *, std::vector<std::size_t>>> &getNextStates() const;
    const std::map<std::set<std::size_t>, std::vector<std::pair<const GroupState *, std::vector<std::size_t>>>> &getPreviousStates() const;

private:
    void addPreviousState(const std::set<std::size_t> key, const GroupState* previousState, std::vector<std::size_t>& groups);
    std::map<std::set<std::size_t>, std::pair<const GroupState *, std::vector<std::size_t>>> nextStates;
    std::map<std::set<std::size_t>, std::vector<std::pair<const GroupState *, std::vector<std::size_t>>>> previousStates;

};

