#pragma once
#include "InitialDFA.h"

#include <algorithm>
#include <iterator>

class NodeAST
{
public:
    NodeAST(NodeAST *pLeft = nullptr, NodeAST *pRight = nullptr) : left(pLeft), right(pRight){}
    virtual ~NodeAST() {  }
    virtual void createInitialDFA(InitialDFA &dfa) const = 0;
    void addParent(NodeAST *pParent) { parent = pParent; }
protected:
    NodeAST *left, *right, *parent;
};

class ConcatenationNodeAST : public NodeAST
{
public:
    ConcatenationNodeAST(NodeAST *pLeft, NodeAST *pRight) : NodeAST(pLeft, pRight)
    {
        pLeft->addParent(this);
        pRight->addParent(this);
    }
    void createInitialDFA(InitialDFA &dfa) const;
};

class TransitionNodeAST : public NodeAST
{
public:
    TransitionNodeAST(std::vector<char>& pCharacters) : NodeAST(nullptr, nullptr)
    {
        std::copy(pCharacters.begin(), pCharacters.end(), std::back_inserter(characters));
    }
    void createInitialDFA(InitialDFA &dfa) const;
    const std::vector<char> &getCharacters(){ return characters; }
private:
    std::vector<char> characters;
};

class AlternativeNodeAST : public NodeAST
{
public:
    AlternativeNodeAST(NodeAST *pLeft, NodeAST *pRight) : NodeAST(pLeft, pRight)
    {
        pLeft->addParent(this);
        pRight->addParent(this);
    }
    void createInitialDFA(InitialDFA &dfa) const;
private:
    std::vector<char> characters;
};

class GroupNodeAST : public NodeAST
{
public:
    GroupNodeAST(NodeAST *pLeft) : NodeAST(pLeft)
    {
        pLeft->addParent(this);
    }
    void createInitialDFA(InitialDFA &dfa) const;
};

class GreedyRepeatitionNodeAST : public NodeAST
{
public:
    GreedyRepeatitionNodeAST(NodeAST *pLeft) : NodeAST(pLeft)
    {
        pLeft->addParent(this);
    }
    void createInitialDFA(InitialDFA &dfa) const;
};

class LazyRepeatitionNodeAST : public NodeAST
{
public:
    LazyRepeatitionNodeAST(NodeAST *pLeft) : NodeAST(pLeft)
    {
        pLeft->addParent(this);
    }
    void createInitialDFA(InitialDFA &dfa) const;
};


class GreedyRepeatitionPlusNodeAST : public NodeAST
{
public:
    GreedyRepeatitionPlusNodeAST(NodeAST *pLeft) : NodeAST(pLeft)
    {
        pLeft->addParent(this);
    }
    void createInitialDFA(InitialDFA &dfa) const;
};

class LazyRepeatitionPlusNodeAST : public NodeAST
{
public:
    LazyRepeatitionPlusNodeAST(NodeAST *pLeft) : NodeAST(pLeft)
    {
        pLeft->addParent(this);
    }
    void createInitialDFA(InitialDFA &dfa) const;
};

class ZeroOrOneNodeAST : public NodeAST
{
public:
    ZeroOrOneNodeAST(NodeAST *pLeft) : NodeAST(pLeft)
    {
        pLeft->addParent(this);
    }
    void createInitialDFA(InitialDFA &dfa) const;
};