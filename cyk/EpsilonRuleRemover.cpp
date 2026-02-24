#include "EpsilonRuleRemover.h"
#include "Utils.h"

#include <algorithm>

namespace
{

SymbolSet FindNullableNonTerminals(const Grammar& grammar)
{
	SymbolSet nullable;
	bool changed = true;

	while (changed)
	{
		changed = false;

		for (const auto& [lhs, productions] : grammar)
		{
			if (nullable.count(lhs))
			{
				continue;
			}

			for (const auto& production : productions)
			{
				if (GrammarUtils::IsEpsilonRule(production))
				{
					nullable.insert(lhs);
					changed = true;
					break;
				}

				bool allNullable = true;
				for (const auto& symbol : production)
				{
					if (symbol != EPSILON && !nullable.count(symbol))
					{
						allNullable = false;
						break;
					}
				}

				if (allNullable && !production.empty())
				{
					nullable.insert(lhs);
					changed = true;
					break;
				}
			}
		}
	}

	return nullable;
}

bool IsNullable(
	const Symbol& symbol,
	const SymbolSet& nullableSymbols)
{
	return nullableSymbols.count(symbol) > 0;
}

Productions GenerateCombinations(
	const Production& production,
	const SymbolSet& nullableSymbols)
{
	Productions result;

	std::vector<size_t> nullableIndices;
	for (size_t i = 0; i < production.size(); ++i)
	{
		if (IsNullable(production[i], nullableSymbols))
		{
			nullableIndices.push_back(i);
		}
	}

	const int combinationCount = 1 << static_cast<int>(nullableIndices.size());

	for (int mask = 0; mask < combinationCount; ++mask)
	{
		Production newProduction;

		for (size_t i = 0; i < production.size(); ++i)
		{
			bool keepSymbol = true;

			if (IsNullable(production[i], nullableSymbols))
			{
				auto it = std::find(nullableIndices.begin(), nullableIndices.end(), i);
				if (it != nullableIndices.end())
				{
					const int idx = static_cast<int>(std::distance(nullableIndices.begin(), it));
					if (((mask >> idx) & 1) == 0)
					{
						keepSymbol = false;
					}
				}
			}

			if (keepSymbol)
			{
				newProduction.push_back(production[i]);
			}
		}

		if (!newProduction.empty())
		{
			if (std::find(result.begin(), result.end(), newProduction) == result.end())
			{
				result.push_back(std::move(newProduction));
			}
		}
	}

	return result;
}
} // namespace

Grammar EpsilonRuleRemover::RemoveEpsilonRules(const Grammar& grammar)
{
	const SymbolSet nullableSymbols = FindNullableNonTerminals(grammar);

	Grammar newGrammar;
	const std::string startSymbol = START_SYMBOL;
	const bool startWasNullable = nullableSymbols.count(startSymbol) > 0;
	std::string newStartSymbol = startSymbol;

	if (startWasNullable)
	{
		newStartSymbol = "S'";
		newGrammar[newStartSymbol].push_back({ startSymbol });
	}

	for (const auto& [lhs, productions] : grammar)
	{
		std::string currentLhs = lhs;
		if (startWasNullable && lhs == startSymbol)
		{
			currentLhs = newStartSymbol;
		}

		for (const auto& production : productions)
		{
			if (GrammarUtils::IsEpsilonRule(production))
			{
				continue;
			}
			const Productions combinations = GenerateCombinations(production, nullableSymbols);

			for (const auto& newProduction : combinations)
			{
				auto& targetProductions = newGrammar[currentLhs];
				if (std::find(targetProductions.begin(), targetProductions.end(), newProduction)
					== targetProductions.end())
				{
					targetProductions.push_back(newProduction);
				}
			}
		}
	}
	return newGrammar;
}
