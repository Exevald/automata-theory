#pragma once

#include "Common.h"

#include <fstream>

class GrammarProcessor
{
public:
	static Symbol CreateSymbol(std::string val);
	static std::vector<Production> ParseFile(const std::string& filename);

private:
	static std::vector<Production> EliminateLeftRecursion(const std::vector<Production>& prods);
};
