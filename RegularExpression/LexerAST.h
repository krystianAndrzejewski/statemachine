#pragma once
#include "RegexAST.h"

class LexerAST : private RegexAST
{
public:

	LexerAST();
	~LexerAST();
	bool addToken(std::string tokenPattern);
	bool createAutomatas(InitialDFA & initDfa, NFA & nfa, RegularDFA & regualarDfa, RegularDFA & intvertedRegularDfa, GroupDFA & groupDfa);
	void getTokenIndexToGroupIndexMap(std::vector<unsigned int>& groupIndexMap) const;
	virtual void print(std::string &output);

private:
	virtual bool manageRepeation(std::size_t & index, std::stack<RegexAST::TypeOperator>& stackOp);

	virtual bool manageOneOrZeroOperator(std::stack<RegexAST::TypeOperator>& stackOp);

	virtual bool manageRepeationPlus(std::size_t & index, std::stack<RegexAST::TypeOperator>& stackOp);

	virtual bool manageGroupBegin(std::stack<RegexAST::TypeOperator>& stackOp);

	std::vector<std::string> tokenDefinitions;

	std::vector<unsigned int> tokenIndexToGroupIndexMap;

	unsigned int numGroups;

	bool isCompiled;
};

