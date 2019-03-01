#pragma once
#include "BaseRegexImpl.h"  
#include "LexerAST.h"
#include "LexerMatch.h"

class Lexer : BaseRegexImpl
{
public:
	Lexer(const std::vector<std::string> &pTokens);
	~Lexer();

	void print(std::string &output);

	LexerMatch match(const std::string &input, std::size_t offset);

private:
	bool processUsingRegularDfa(const std::string & input, std::size_t offset, std::size_t & lastInitialState, std::size_t & lastFinalState);

	LexerAST ast;
	bool success;
};

