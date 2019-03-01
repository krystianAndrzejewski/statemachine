#pragma once
#include <string>
#include <vector>

class LexerMatch
{
public:
	LexerMatch(int pIndex, const std::string& pResult);
	LexerMatch();
	~LexerMatch();
	bool isEmpty() { return empty; }
	int getIndex() { return index; }
	std::string &getResult() { return result; };
private:
	int index;
	std::string result;
	bool empty;
};

