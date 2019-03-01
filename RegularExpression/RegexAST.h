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

    virtual void print(std::string &output);

    bool isGreedy() { return greedyRegex; }

protected:
	RegexAST() {}

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

	bool compile(std::vector<NodeAST *> &nodes);

	virtual bool manageAlternative(std::size_t index, std::stack<RegexAST::TypeOperator>& stackOp);

	virtual bool manageEscapedTransition(std::size_t &index, std::stack<RegexAST::TypeOperator>& stackOp, const std::string &escapeSign);

	virtual bool manageTransition(std::size_t index, std::stack<RegexAST::TypeOperator>& stackOp);

	virtual bool manageMultiTransition(std::size_t & index, std::stack<RegexAST::TypeOperator>& stackOp, const std::string & escapeSign);

	virtual bool manageGroupBegin(std::stack<RegexAST::TypeOperator>& stackOp);

	virtual bool manageGroupEnd(std::stack<RegexAST::TypeOperator>& stackOp);

	virtual bool manageRepeation(std::size_t & index, std::stack<RegexAST::TypeOperator>& stackOp);

	virtual bool manageOneOrZeroOperator(std::stack<RegexAST::TypeOperator>& stackOp);

	virtual bool manageRepeationPlus(std::size_t & index, std::stack<RegexAST::TypeOperator>& stackOp);

	bool createPostfixRepresentation();

	bool createASTtree(std::vector<NodeAST *> &pNodes);

	void processIngredient(
		std::stack<TypeOperator> &stackOp,
		TypeOperator pOperator);

	void processTwoArgumentOperator(
		std::stack<TypeOperator> &stackOp,
		TypeOperator pOperator);

	void processOneArgumentOperator(
		TypeOperator pOperator);

	std::string pattern;
    static const std::map<TypeOperator, std::size_t> operatorPrority;
    std::vector<RegexAST::TypeOperator> operators;
    std::vector<std::vector<char>> result;
    std::vector<NodeAST *> nodes;
    bool greedyRegex;
	std::string escapeSign;
};

