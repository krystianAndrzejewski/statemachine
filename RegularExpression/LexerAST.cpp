#include "LexerAST.h"
#include <sstream>


LexerAST::LexerAST() : numGroups(1), isCompiled(false)
{
	std::string escapeSign = "\\()+*|[]";
	greedyRegex = true;
}

LexerAST::~LexerAST()
{
}

bool LexerAST::addToken(std::string tokenPattern)
{
	if (isCompiled)
	{
		return false;
	}
	unsigned int previousNumGroups = numGroups;
	pattern = tokenPattern;
	std::vector<NodeAST *> astNodes;
	bool result = RegexAST::compile(astNodes);
	if (result)
	{
		std::size_t numLastNode = nodes.size() - 1;
		std::copy(astNodes.begin(), astNodes.end(), std::back_inserter(nodes));
		nodes.push_back(new GroupNodeAST(nodes[nodes.size() - 1]));
		tokenIndexToGroupIndexMap.push_back(previousNumGroups);
		numGroups++;
		if (previousNumGroups > 1)
		{
			nodes.push_back(new AlternativeNodeAST(nodes[numLastNode], nodes[nodes.size() - 1]));
		}
	}
	else
	{
		numGroups = previousNumGroups;
	}
	return result;
}

bool LexerAST::createAutomatas(
	InitialDFA &initDfa,
	NFA &nfa,
	RegularDFA &regualarDfa,
	RegularDFA &intvertedRegularDfa,
	GroupDFA &groupDfa)
{
	isCompiled = true;
	return RegexAST::createAutomatas(
		initDfa,
		nfa,
		regualarDfa,
		intvertedRegularDfa,
		groupDfa);
}

void LexerAST::getTokenIndexToGroupIndexMap(std::vector<unsigned int>& groupIndexMap) const
{
	groupIndexMap.assign(tokenIndexToGroupIndexMap.begin(), tokenIndexToGroupIndexMap.end());
}

void LexerAST::print(std::string & output)
{
	std::stringstream stream;
	stream << "Group indices for following tokens: ";
	unsigned int i = 0;
	for (auto groupIndex : tokenIndexToGroupIndexMap)
	{
		stream << "( " << i++ << " -> " << groupIndex << " ), ";
	}
	stream << "\n\n";
	output += stream.str();
}

bool LexerAST::manageRepeation(std::size_t & index, std::stack<RegexAST::TypeOperator>& stackOp)
{
	if (result.size() == 0 || (!stackOp.empty() && stackOp.top() == alternative) || operators.back() > alternative)
	{
		return false;
	}
	processOneArgumentOperator(
			greedyRepeation);
	return true;
}

bool LexerAST::manageOneOrZeroOperator(std::stack<RegexAST::TypeOperator>& stackOp)
{
	return false;
}

bool LexerAST::manageRepeationPlus(std::size_t & index, std::stack<RegexAST::TypeOperator>& stackOp)
{
	if (result.size() == 0 || (!stackOp.empty() && stackOp.top() == alternative) || operators.back() > alternative)
	{
		return false;
	}
	processOneArgumentOperator(
		greedyRepeationPlus);
	return true;
}

bool LexerAST::manageGroupBegin(std::stack<RegexAST::TypeOperator>& stackOp)
{
	numGroups++;
	return RegexAST::manageGroupBegin(stackOp);
}
