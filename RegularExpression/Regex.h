#pragma once
#include "BaseRegexImpl.h"  
#include "RegexAST.h"
#include "RegexMatch.h"

class Regex : BaseRegexImpl
{
public:
    Regex(const std::string &pPattern);
    ~Regex();

    void print(std::string &output);
    RegexMatch match(const std::string &input, std::size_t offset = 0);

private:

    RegexMatch useDFA(const std::string &input, std::size_t offset);
    RegexMatch useNFA(const std::string &input, std::size_t offset);

    RegexAST ast;

    bool success;
};

