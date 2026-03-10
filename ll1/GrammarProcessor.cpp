#include "GrammarProcessor.h"

#include <fstream>
#include <map>
#include <sstream>

namespace
{

bool IsNonTerminal(const std::string& s)
{
	return s.size() > 2 && s.front() == '<' && s.back() == '>';
}

} // namespace

Symbol GrammarProcessor::CreateSymbol(std::string val)
{
	std::erase(val, ' ');
	if (val == "e" || val.empty())
	{
		return { "e", SymbolType::Epsilon };
	}
	if (val == "⊥" || val == "$")
	{
		return { "⊥", SymbolType::EndMarker };
	}
	if (IsNonTerminal(val))
	{
		return { val, SymbolType::NonTerminal };
	}
	return { val, SymbolType::Terminal };
}

std::vector<Production> GrammarProcessor::ParseFile(const std::string& filename)
{
	std::vector<Production> rawProductions;
	std::ifstream file(filename);
	if (!file.is_open())
	{
		throw std::runtime_error("File not found: " + filename);
	}

	std::string line;
	while (std::getline(file, line))
	{
		if (line.empty())
		{
			continue;
		}

		size_t arrowPos = line.find("->");
		if (arrowPos == std::string::npos)
		{
			continue;
		}

		Symbol lhs = CreateSymbol(line.substr(0, arrowPos));
		std::string rhsPart = line.substr(arrowPos + 2);

		std::stringstream ss(rhsPart);
		std::string segment;
		while (std::getline(ss, segment, '|'))
		{
			std::stringstream segmentStream(segment);
			std::string token;
			Production prod;
			prod.lhs = lhs;

			while (segmentStream >> token)
			{
				prod.rhs.push_back(CreateSymbol(token));
			}
			if (prod.rhs.empty())
			{
				prod.rhs.push_back(CreateSymbol("e"));
			}
			rawProductions.push_back(prod);
		}
	}
	return EliminateLeftRecursion(rawProductions);
}

std::vector<Production> GrammarProcessor::EliminateLeftRecursion(const std::vector<Production>& prods)
{
	std::map<Symbol, std::vector<std::vector<Symbol>>> grouped;
	std::vector<Symbol> order;
	for (const auto& p : prods)
	{
		if (std::ranges::find(order, p.lhs) == order.end())
		{
			order.push_back(p.lhs);
		}
		grouped[p.lhs].push_back(p.rhs);
	}

	std::vector<Production> result;
	for (const auto& lhs : order)
	{
		auto& rhss = grouped[lhs];
		std::vector<std::vector<Symbol>> recursive, nonRecursive;
		for (const auto& rhs : rhss)
		{
			if (!rhs.empty() && rhs[0] == lhs)
			{
				recursive.emplace_back(rhs.begin() + 1, rhs.end());
			}
			else
			{
				nonRecursive.push_back(rhs);
			}
		}

		if (recursive.empty())
		{
			for (const auto& rhs : rhss)
			{
				result.push_back({ lhs, rhs });
			}
		}
		else
		{
			std::string baseName = lhs.value.substr(0, lhs.value.size() - 1);
			Symbol newNt = { baseName + "'>", SymbolType::NonTerminal };

			for (auto beta : nonRecursive)
			{
				if (beta.size() == 1 && beta[0].IsEpsilon())
				{
					beta.clear();
				}
				beta.push_back(newNt);
				result.push_back({ lhs, beta });
			}
			for (auto alpha : recursive)
			{
				alpha.push_back(newNt);
				result.push_back({ newNt, alpha });
			}
			result.push_back({ newNt, { CreateSymbol("e") } });
		}
	}
	return result;
}