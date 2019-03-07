#include "DeterministicFiniteAutomata.h"
#include "NonDeterministicFiniteAutomata.h"

int main(char **argc, int argv)
{
    std::vector<std::size_t> finalStates = { 1, 2, 4 };
    DeterministicFiniteAutomata<char> dfa(6, 0, finalStates);
    dfa.linkStates(0, 3, '0');
    dfa.linkStates(3, 0, '0');
    dfa.linkStates(3, 4, '1');
    dfa.linkStates(0, 1, '1');
    dfa.linkStates(4, 2, '0');
    dfa.linkStates(4, 5, '1');
    dfa.linkStates(1, 5, '1');
    dfa.linkStates(1, 2, '0');
    dfa.linkStates(2, 5, '1');
    dfa.linkStates(2, 2, '0');
    dfa.linkStates(5, 5, '0');
    dfa.linkStates(5, 5, '1');
    dfa.simplifyDFA();
    NonDeterministicFiniteAutomata<char> nfa(4, 0, {2});
    nfa.linkStates(0, 1, 'a');
    nfa.linkStates(0, 3, 'c');
    nfa.linkStates(1, 0);
    nfa.linkStates(1, 2, 'b');
    nfa.linkStates(2, 1, 'a');
    nfa.linkStates(3, 2, 'c');
    nfa.linkStates(3, 2);
	DeterministicFiniteAutomata<char>* tmp = nfa.createDFA();
	delete tmp;
    return 0;
}