#include "UnitRuleRemover.h"

SymbolSet UnitRuleRemover::ExtractNonTerminals(const Grammar& grammar)
{
	SymbolSet nonTerminals;
	for (const auto& [lhs, _] : grammar)
	{
		nonTerminals.insert(lhs);
	}
	return nonTerminals;
}

bool UnitRuleRemover::IsUnitRule(
	const Production& production,
	const SymbolSet& nonTerminals)
{
	return production.size() == 1 && nonTerminals.count(production[0]) > 0;
}

Grammar UnitRuleRemover::RemoveUnitRules(Grammar grammar)
{
	const SymbolSet nonTerminals = ExtractNonTerminals(grammar);

	bool changed = true;

	while (changed)
	{
		changed = false;

		for (auto& [lhs, productions] : grammar)
		{
			Productions newProductions;

			for (const auto& production : productions)
			{
				if (IsUnitRule(production, nonTerminals))
				{
					const Symbol& rhs = production[0];

					if (grammar.count(rhs))
					{
						for (const auto& subProduction : grammar.at(rhs))
						{
							if (subProduction.size() != 1 || subProduction[0] != lhs)
							{
								newProductions.push_back(subProduction);
								changed = true;
							}
						}
					}
				}
				else
				{
					newProductions.push_back(production);
				}
			}

			productions = std::move(newProductions);
		}
	}

	return grammar;
}
