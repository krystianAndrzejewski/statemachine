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
    compile();
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
    std::string intend = "    ";
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

void RegexAST::compile()
{
    bool success = createPostfixRepresentation();
    if (success)
    {
        success = createASTtree();
    }
    if (!success)
    {
        for (auto node : nodes)
        {
            delete node;
        }
        nodes.clear();
    }
}

bool RegexAST::createASTtree()
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
            nodes.push_back(new TransitionNodeAST(result[operandNumber]));
            operandNumber++;
            break;
        case concatenation:
            nodes.push_back(new ConcatenationNodeAST(nodes[args[1]], nodes[args[0]]));
            break;
        case alternative:
            nodes.push_back(new AlternativeNodeAST(nodes[args[1]], nodes[args[0]]));
            greedyRegex = false;
            break;
        case lazyRepeation:
            nodes.push_back(new LazyRepeatitionNodeAST(nodes[args[0]]));
            greedyRegex = false;
            break;
        case lazyRepeationPlus:
            nodes.push_back(new LazyRepeatitionPlusNodeAST(nodes[args[0]]));
            greedyRegex = false;
            break;
        case greedyRepeation:
            nodes.push_back(new GreedyRepeatitionNodeAST(nodes[args[0]]));
            break;
        case greedyRepeationPlus:
            nodes.push_back(new GreedyRepeatitionPlusNodeAST(nodes[args[0]]));
            break;
        case zeroOrOne:
            nodes.push_back(new ZeroOrOneNodeAST(nodes[args[0]]));
            break;
        case group:
            nodes.push_back(new GroupNodeAST(nodes[args[0]]));
            break;
        default:
            break;
        }
        stackOp.push(i);
    }
    return true;
}

bool RegexAST::createPostfixRepresentation()
{
    std::stack<RegexAST::TypeOperator> stackOp;
    std::string escapeSign = "\\()+*?|";
    for (std::size_t i = 0; i < pattern.size(); i++)
    {
        switch (pattern[i])
        {
        case('|') :
            if (result.size() == 0 || (!stackOp.empty() && stackOp.top() == alternative))
            {
                return false;
            }
                  processTwoArgumentOperator(
                      stackOp,
                      alternative);
                  break;
        case('\\') :
            i++;
            if (i < pattern.size() && escapeSign.find(pattern[i]) != std::string::npos)
            {
                result.push_back({ pattern[i] });
                operators.push_back(transition);
                processIngredient(
                    stackOp,
                    concatenation);
            }
            else
            {
                return false;
            }
            break;
        case('(') :
            processIngredient(
                stackOp,
                group);
            break;
        case(')') :
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
            break;
        case('*') :
            if (result.size() == 0 || (!stackOp.empty() && stackOp.top() == alternative) || operators.back() > alternative)
            {
                return false;
            }
                  if (i + 1 < pattern.size() && pattern[i + 1] == '?')
                  {
                      i++;
                      processOneArgumentOperator(
                          lazyRepeation);
                  }
                  else
                  {
                      processOneArgumentOperator(
                          greedyRepeation);
                  }
                  break;
        case('?') :
            if (result.size() == 0 || (!stackOp.empty() && stackOp.top() == alternative) || operators.back() > alternative)
            {
                return false;
            }
                  processOneArgumentOperator(
                      zeroOrOne);
                  break;
        case('+') :
            if (result.size() == 0 || (!stackOp.empty() && stackOp.top() == alternative) || operators.back() > alternative)
            {
                return false;
            }
                  if (i + 1 < pattern.size() && pattern[i + 1] == '?')
                  {
                      i++;
                      processOneArgumentOperator(
                          lazyRepeationPlus);
                  }
                  else
                  {
                      processOneArgumentOperator(
                          greedyRepeationPlus);
                  }
                  break;
        default:
            result.push_back({ pattern[i] });
            operators.push_back(transition);
            processIngredient(
                stackOp,
                concatenation);
            break;
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