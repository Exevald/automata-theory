#pragma once

#include "Machines.h"

#include <string>
#include <vector>
#include <set>
#include <regex>

enum class GrammarType
{
	RightLinear,
	LeftLinear,
	Unknown
};

using ProductionRule = std::pair<std::string, std::string>;
using ProductionRules = std::vector<ProductionRule>;

class Grammar
{
public:
	Grammar() = default;

	static Grammar FromString(const std::string& grammarText);
	static Grammar FromFile(const std::string& filename);

	[[nodiscard]] MealyMachine ToNFA() const;

	[[nodiscard]] GrammarType GetType() const { return m_type; }

private:
	std::set<std::string> m_nonTerminals;
	ProductionRules m_rules;
	GrammarType m_type = GrammarType::Unknown;
	std::string m_startSymbol;

	void DetermineType();
};