#include "CNFConverter.h"
#include "EpsilonRuleRemover.h"
#include "UnitRuleRemover.h"
#include "Utils.h"

namespace
{
Symbol GenerateNewNonTerminal(
	const std::string& prefix,
	int& counter)
{
	return prefix + std::to_string(counter++);
}

Grammar ReplaceTerminalsInLongRules(Grammar grammar)
{
	std::map<Symbol, Symbol> terminalMap;
	int counter = 0;

	for (auto& [lhs, productions] : grammar)
	{
		for (auto& production : productions)
		{
			if (production.size() <= 1)
			{
				continue;
			}

			for (auto& symbol : production)
			{
				if (GrammarUtils::IsTerminal(symbol))
				{
					if (terminalMap.find(symbol) == terminalMap.end())
					{
						const Symbol newNonTerminal = GenerateNewNonTerminal("X", counter);
						terminalMap[symbol] = newNonTerminal;
						grammar[newNonTerminal].push_back({ symbol });
					}
					symbol = terminalMap[symbol];
				}
			}
		}
	}

	return grammar;
}

Grammar BinariizeLongRules(Grammar grammar)
{
	int counter = 0;
	Grammar newGrammar;

	for (const auto& [lhs, productions] : grammar)
	{
		for (const auto& production : productions)
		{
			Production currentProd = production;

			while (currentProd.size() > 2)
			{
				const Symbol newNonTerminal = GenerateNewNonTerminal("Z", counter);

				Production binaryRule = {
					currentProd[currentProd.size() - 2],
					currentProd.back()
				};

				currentProd.pop_back();
				currentProd.pop_back();
				currentProd.push_back(newNonTerminal);

				newGrammar[newNonTerminal].push_back(std::move(binaryRule));
			}

			newGrammar[lhs].push_back(std::move(currentProd));
		}
	}

	return newGrammar;
}
} // namespace

Grammar CNFConverter::ConvertToCNF(Grammar grammar)
{
	grammar = EpsilonRuleRemover::RemoveEpsilonRules(grammar);
	grammar = UnitRuleRemover::RemoveUnitRules(grammar);
	grammar = ReplaceTerminalsInLongRules(grammar);
	grammar = BinariizeLongRules(grammar);

	return grammar;
}
