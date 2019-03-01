#include "RegexAST.h"

#include <sstream>

const std::map<RegexAST::TypeOperator, std::size_t> RegexAST::operatorPrority = {
    { transition, 0 },
    { greedyRepeation, 3 },
    { lazyRepeation, 3 },
    { greedyRepeationPlus, 3 },
    { lazyRepeationPlus, 3 },
    { zeroOrOne, 3 },
    { concatenation, 2 },
    { alternative, 1 },
    { group, 2 }
};


RegexAST::RegexAST(const std::string &regex) : pattern(regex), greedyRegex(true)
{
	escapeSign = "\\()+*?|[]";
    compile(nodes);
}


RegexAST::~RegexAST()
{
    for (auto node : nodes)
    {
        delete node;
    }
}

void RegexAST::print(std::string &output)
{
    std::size_t i = 0;
    std::stringstream stream;
    stream << "A abstract tree state of the regular expression.\n\n";
    stream << "Input pattern: " << pattern << "\n\n";
    std::size_t operandNumber = 0;
    stream << "The parse result - postfix representation:\n";
    for (std::size_t i = 0; i < operators.size(); i++)
    {
        switch (operators[i])
        {
        case transition:
            stream << "transition: [";
            if (operandNumber < result.size())
            {
                for (auto character : result[operandNumber])
                {
                    stream << character << ", ";
                }
            }
            operandNumber++;
            stream << "], ";
            break;
        case concatenation:
            stream << "concatenation, ";
            break;
        case alternative:
            stream << "alternative, ";
            break;
        case lazyRepeation:
            stream << "lazyRepeation, ";
            break;
        case lazyRepeationPlus:
            stream << "lazyRepeationPlus, ";
            break;
        case greedyRepeation:
            stream << "greedyRepeation, ";
            break;
        case greedyRepeationPlus:
            stream << "greedyRepeationPlus, ";
            break;
        case zeroOrOne:
            stream << "zeroOrOne, ";
            break;
        case group:
            stream << "group, ";
            break;
        default:
            break;
        }
    }
    stream << "\n\n";
    output += stream.str();
}

bool RegexAST::createAutomatas(
    InitialDFA &initDfa, 
    NFA &nfa,
    RegularDFA &regualarDfa, 
    RegularDFA &intvertedRegularDfa, 
    GroupDFA &groupDfa) const
{
    if (!nodes.empty())
    {
        nodes.back()->createInitialDFA(initDfa);
        if (greedyRegex)
        {
            nfa.createFromInitialDfa(initDfa);
            regualarDfa.createFromInitialNfa(nfa);
            groupDfa.createFromInitialDFA(initDfa, intvertedRegularDfa);
        }
        return true;
    }
    return false;
}

bool RegexAST::compile(std::vector<NodeAST *> &pNodes)
{
    bool success = createPostfixRepresentation();
    if (success)
    {
        success = createASTtree(pNodes);
    }
    if (!success)
    {
        for (auto node : nodes)
        {
            delete node;
        }
        nodes.clear();
    }
	return success;
}

bool RegexAST::createASTtree(std::vector<NodeAST *> &pNodes)
{
    std::size_t operandNumber = 0;
    std::stack<std::size_t> stackOp;
    for (std::size_t i = 0; i < operators.size(); i++)
    {
        std::size_t args[2] = { operators.size(), operators.size() };
        switch (operators[i])
        {
        case transition:
            if (operandNumber >= result.size())
            {
                return false;
            }
            break;
        case concatenation:
        case alternative:
            for (std::size_t j = 0; j < 2; j++)
            {
                if (!stackOp.empty())
                {
                    args[j] = stackOp.top();
                    stackOp.pop();
                }
                if (args[j] == operators.size())
                {
                    return false;
                }
            }
            break;
        case lazyRepeation:
        case lazyRepeationPlus:
        case greedyRepeation:
        case greedyRepeationPlus:
        case zeroOrOne:
        case group:
            if (!stackOp.empty())
            {
                args[0] = stackOp.top();
                stackOp.pop();
            }
            if (args[0] == operators.size())
            {
                return false;
            }
            break;
        default:
            break;
        }
        switch (operators[i])
        {
        case transition:
			pNodes.push_back(new TransitionNodeAST(result[operandNumber]));
            operandNumber++;
            break;
        case concatenation:
			pNodes.push_back(new ConcatenationNodeAST(pNodes[args[1]], pNodes[args[0]]));
            break;
        case alternative:
			pNodes.push_back(new AlternativeNodeAST(pNodes[args[1]], pNodes[args[0]]));
			greedyRegex = false;
            break;
        case lazyRepeation:
			pNodes.push_back(new LazyRepeatitionNodeAST(pNodes[args[0]]));
            greedyRegex = false;
            break;
        case lazyRepeationPlus:
			pNodes.push_back(new LazyRepeatitionPlusNodeAST(pNodes[args[0]]));
            greedyRegex = false;
            break;
        case greedyRepeation:
			pNodes.push_back(new GreedyRepeatitionNodeAST(pNodes[args[0]]));
            break;
        case greedyRepeationPlus:
			pNodes.push_back(new GreedyRepeatitionPlusNodeAST(pNodes[args[0]]));
            break;
        case zeroOrOne:
			pNodes.push_back(new ZeroOrOneNodeAST(pNodes[args[0]]));
            break;
        case group:
			pNodes.push_back(new GroupNodeAST(pNodes[args[0]]));
            break;
        default:
            break;
        }
        stackOp.push(i);
    }
    return true;
}

bool RegexAST::manageAlternative(std::size_t index, std::stack<RegexAST::TypeOperator>& stackOp)
{
	if (result.size() == 0 || (!stackOp.empty() && stackOp.top() == alternative))
	{
		return false;
	}
	processTwoArgumentOperator(
		stackOp,
		alternative);
	return true;
}

bool RegexAST::manageEscapedTransition(std::size_t &index, std::stack<RegexAST::TypeOperator>& stackOp, const std::string &escapeSign)
{
	index++;
	if (index < pattern.size() && escapeSign.find(pattern[index]) != std::string::npos)
	{
		manageTransition(index, stackOp);
	}
	else
	{
		return false;
	}
	return true;
}

bool RegexAST::manageTransition(std::size_t index, std::stack<RegexAST::TypeOperator>& stackOp)
{
	result.push_back({ pattern[index] });
	operators.push_back(transition);
	processIngredient(
		stackOp,
		concatenation);
	return true;
}

bool RegexAST::manageMultiTransition(std::size_t &index, std::stack<RegexAST::TypeOperator>& stackOp, const std::string &escapeSign)
{
	std::set<char> characters;
	index++;
	bool isNegate = false;
	if (index < pattern.size() && pattern[index] == '^')
	{
		isNegate = true;
	}

	while (index < pattern.size() && pattern[index] != ']')
	{
		const char *actualCharacter = nullptr;
		if (pattern[index] == '\\')
		{
			if (pattern[index] == ']')
			{
				actualCharacter = &(pattern[index]);
			}
			else
			{
				return false;
			}
		}
		else
		{
			actualCharacter = &(pattern[index]);
		}

		index++;

		if (index < pattern.size() && pattern[index] == '-')
		{
			index += 1;
			if (pattern[index] == '\\')
			{
				if (pattern[index] == ']')
				{
					actualCharacter = &(pattern[index]);
				}
				else
				{
					return false;
				}
				index++;
			}
			if (index < pattern.size())
			{
				if (*actualCharacter >= pattern[index])
				{
					return false;
				}
				for (char c = *actualCharacter; c <= pattern[index]; c++)
				{
					characters.insert(c);
				}
			}
		}
		else
		{
			characters.insert(*actualCharacter);
		}
	}
	if (isNegate)
	{
		std::vector<char> negatedCharacters;
		for (char c = 0; c <= CHAR_MAX; c++)
		{
			if (characters.find(c) == characters.end())
			{
				negatedCharacters.push_back(c);
			}
		}
		if (negatedCharacters.size() == 0)
		{
			return false;
		}
		result.push_back(negatedCharacters);
	}
	else
	{
		if (characters.size() == 0)
		{
			return false;
		}
		result.push_back(std::vector<char>(characters.begin(), characters.end()));
	}
	operators.push_back(transition);
	processIngredient(
		stackOp,
		concatenation);
	return pattern[index] == ']';
}

bool RegexAST::manageGroupBegin(std::stack<RegexAST::TypeOperator>& stackOp)
{
	processIngredient(
		stackOp,
		group);
	return true;
}

bool RegexAST::manageGroupEnd(std::stack<RegexAST::TypeOperator>& stackOp)
{
	while (!stackOp.empty() && operatorPrority.at(stackOp.top()) != operatorPrority.at(group))
	{
		if (stackOp.top() != transition)
		{
			operators.push_back(stackOp.top());
		}
		stackOp.pop();
	}
	if (!stackOp.empty())
	{
		operators.push_back(stackOp.top());
		stackOp.pop();
	}
	else
	{
		return false;
	}
	return true;
}

bool RegexAST::manageRepeation(std::size_t &index, std::stack<RegexAST::TypeOperator>& stackOp)
{
	if (result.size() == 0 || (!stackOp.empty() && stackOp.top() == alternative) || operators.back() > alternative)
	{
		return false;
	}
	if (index + 1 < pattern.size() && pattern[index + 1] == '?')
	{
		index++;
		processOneArgumentOperator(
			lazyRepeation);
	}
	else
	{
		processOneArgumentOperator(
			greedyRepeation);
	}
	return true;
}

bool RegexAST::manageOneOrZeroOperator(std::stack<RegexAST::TypeOperator>& stackOp)
{
	if (result.size() == 0 || (!stackOp.empty() && stackOp.top() == alternative) || operators.back() > alternative)
	{
		return false;
	}
	processOneArgumentOperator(
		zeroOrOne);
	return true;
}

bool RegexAST::manageRepeationPlus(std::size_t &index, std::stack<RegexAST::TypeOperator>& stackOp)
{
	if (result.size() == 0 || (!stackOp.empty() && stackOp.top() == alternative) || operators.back() > alternative)
	{
		return false;
	}
	if (index + 1 < pattern.size() && pattern[index + 1] == '?')
	{
		index++;
		processOneArgumentOperator(
			lazyRepeationPlus);
	}
	else
	{
		processOneArgumentOperator(
			greedyRepeationPlus);
	}
	return true;
}

bool RegexAST::createPostfixRepresentation()
{
    std::stack<RegexAST::TypeOperator> stackOp;
	operators.clear();
	result.clear();
    for (std::size_t i = 0; i < pattern.size(); i++)
    {
		bool success = true;
        switch (pattern[i])
        {
        case('|') :
			success = manageAlternative(i, stackOp);
            break;
        case('\\') :
			success = manageEscapedTransition(i, stackOp, escapeSign);
            break;
        case('(') :
			success = manageGroupBegin(stackOp);
            break;
        case(')') :
			success = manageGroupEnd(stackOp);
            break;
        case('*') :
			success = manageRepeation(i, stackOp);
			break;
        case('?') :
			success = manageOneOrZeroOperator(stackOp);
            break;
        case('+') :
			success = manageRepeationPlus(i, stackOp);
            break;
		case ('['):
			success = manageMultiTransition(i, stackOp, escapeSign);
			break;
        default:
			success = manageTransition(i, stackOp);
            break;
        }
		if (success == false)
		{
			return false;
		}
    }
    while (!stackOp.empty())
    {
        if (stackOp.top() == group)
        {
            return false;
        }
        if (operatorPrority.at(stackOp.top()) != operatorPrority.at(transition))
        {
            operators.push_back(stackOp.top());
        }
        stackOp.pop();
    }
    return true;
}

void RegexAST::processTwoArgumentOperator(
    std::stack<RegexAST::TypeOperator> &stackOp,
    RegexAST::TypeOperator pOperator)
{
    while (!stackOp.empty() &&
        (operatorPrority.at(pOperator) < operatorPrority.at(stackOp.top()) &&
        stackOp.top() != group))
    {
        if (stackOp.top() != transition)
        {
            operators.push_back(stackOp.top());
        }
        stackOp.pop();
    }
    stackOp.push(pOperator);
}

void RegexAST::processOneArgumentOperator(
    RegexAST::TypeOperator pOperator)
{
    operators.push_back(pOperator);
}

void RegexAST::processIngredient(
    std::stack<RegexAST::TypeOperator> &stackOp,
    RegexAST::TypeOperator pOperator)
{
    bool wasConcatenation = false;
    while (!stackOp.empty() &&
        ((operatorPrority.at(pOperator) <= operatorPrority.at(stackOp.top()) ||
        stackOp.top() == RegexAST::transition) &&
        stackOp.top() != RegexAST::group))
    {
        if (stackOp.top() == RegexAST::transition)
        {
            stackOp.pop();
            wasConcatenation = true;
            break;
        }
        if (stackOp.top() == concatenation)
        {
            wasConcatenation = true;
        }
        operators.push_back(stackOp.top());
        stackOp.pop();
    }
    if (wasConcatenation)
    {
        stackOp.push(concatenation);
    }
    else
    {
        stackOp.push(transition);
    }
    if (pOperator != concatenation)
    {
        stackOp.push(pOperator);
    }
}