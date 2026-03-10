#pragma once

#include <set>
#include <string>
#include <vector>

enum class SymbolType
{
	NonTerminal,
	Terminal,
	Epsilon,
	EndMarker
};

struct Symbol
{
	std::string value;
	SymbolType type{};

	bool operator<(const Symbol& other) const
	{
		if (type != other.type)
		{
			return type < other.type;
		}
		return value < other.value;
	}
	bool operator==(const Symbol& other) const
	{
		return value == other.value && type == other.type;
	}
	[[nodiscard]] bool IsEpsilon() const { return type == SymbolType::Epsilon || value == "e"; }
};

struct Production
{
	Symbol lhs;
	std::vector<Symbol> rhs;
	std::set<Symbol> guideSet;
	int rhsStartRow = 0;
};