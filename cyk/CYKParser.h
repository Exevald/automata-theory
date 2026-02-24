#pragma once

#include "GrammarTypes.h"

#include <vector>

namespace CYKParser
{
	bool Parse(const Grammar& grammar, const std::string& input);
	bool ParseWithTable(const Grammar& grammar, const std::string& input);
};
