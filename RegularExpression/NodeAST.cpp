#include "NodeAST.h"


void ConcatenationNodeAST::createInitialDFA(InitialDFA &dfa) const
{

    if (left != nullptr)
    {
        left->createInitialDFA(dfa);
    }
    if (right != nullptr && dynamic_cast<TransitionNodeAST*>(right) == nullptr)
    {
        InitialDFA dfaRight;
        right->createInitialDFA(dfaRight);
        dfa.concatenation(dfaRight);
    }
    else if (right != nullptr)
    {
        dfa.transition(dynamic_cast<TransitionNodeAST*>(right)->getCharacters());
    }
}

void TransitionNodeAST::createInitialDFA(InitialDFA &dfa) const
{
    dfa.transition(characters);
}

void AlternativeNodeAST::createInitialDFA(InitialDFA &dfa) const
{
    InitialDFA dfaRight;
    if (left != nullptr)
    {
        left->createInitialDFA(dfa);
    }
    if (right != nullptr)
    {
        right->createInitialDFA(dfaRight);
    }
    dfa.alternative(dfaRight);
}

void GroupNodeAST::createInitialDFA(InitialDFA &dfa) const
{
    if (left != nullptr)
    {
        left->createInitialDFA(dfa);
    }
    dfa.group();
}

void GreedyRepeatitionNodeAST::createInitialDFA(InitialDFA &dfa) const
{
    if (left != nullptr)
    {
        left->createInitialDFA(dfa);
    }
    dfa.greedyRepeation();
}

void LazyRepeatitionNodeAST::createInitialDFA(InitialDFA &dfa) const
{
    if (left != nullptr)
    {
        left->createInitialDFA(dfa);
    }
    dfa.lazyRepeation();
}

void GreedyRepeatitionPlusNodeAST::createInitialDFA(InitialDFA &dfa) const
{
    if (left != nullptr)
    {
        left->createInitialDFA(dfa);
    }
    dfa.greedyRepeationPlus();
}

void LazyRepeatitionPlusNodeAST::createInitialDFA(InitialDFA &dfa) const
{
    if (left != nullptr)
    {
        left->createInitialDFA(dfa);
    }
    dfa.lazyRepeationPlus();
}

void ZeroOrOneNodeAST::createInitialDFA(InitialDFA &dfa) const
{
    if (left != nullptr)
    {
        left->createInitialDFA(dfa);
    }
    dfa.lazyRepeationPlus();
}