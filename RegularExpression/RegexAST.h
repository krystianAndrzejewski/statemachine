#pragma once
#include <string>
#include <map>
#include <stack>
#include <vector>
#include "NodeAST.h"
#include "GroupDFA.h"

class RegexAST
{
public:
    RegexAST(const std::string &regex);
    ~RegexAST();
    bool createAutomatas(
        InitialDFA &initDfa,
        NFA &nfa,
        RegularDFA &regualarDfa,
        RegularDFA &intvertedRegularDfa,
        GroupDFA &groupDfa) const;

    void print(std::string &output);

    bool isGreedy() { return greedyRegex; }

private:
    enum TypeOperator
    {
        transition,
        group,
        concatenation,
        alternative,
        greedyRepeation,
        lazyRepeation,
        greedyRepeationPlus,
        lazyRepeationPlus,
        zeroOrOne
    };

    void compile();

    void processIngredient(
        std::stack<TypeOperator> &stackOp,
        TypeOperator pOperator);

    void processTwoArgumentOperator(
        std::stack<TypeOperator> &stackOp,
        TypeOperator pOperator);

    void processOneArgumentOperator(
        TypeOperator pOperator);

    bool createPostfixRepresentation();

    bool createASTtree();

    std::string pattern;
    static const std::map<TypeOperator, std::size_t> operatorPrority;
    std::vector<RegexAST::TypeOperator> operators;
    std::vector<std::vector<char>> result;
    std::vector<NodeAST *> nodes;
    bool greedyRegex;

};

