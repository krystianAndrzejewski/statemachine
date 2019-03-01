#include "LexerMatch.h"



LexerMatch::LexerMatch(int pIndex, const std::string& pResult) : result(pResult), index(pIndex), empty(false)
{
}

LexerMatch::LexerMatch() : index(-1), empty(true)
{
}


LexerMatch::~LexerMatch()
{
}
