#pragma once
#include <string>
#include <vector>

class RegexMatch
{
public:
    RegexMatch(const std::vector<std::string>& pResult, std::size_t pOffset);
    RegexMatch();
    ~RegexMatch();
    bool isEmpty(){ return empty; }
    std::size_t getOffset() { return offset; }
    bool getGroup(std::string &pResult, std::size_t number);
    void getGroups(std::vector<std::string> &pResult);
private:
    std::vector<std::string> groups;
    std::size_t offset;
    bool empty;
};

