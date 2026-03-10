#include "TableGenerator.h"
#include "AnalysisEngine.h"

#include <iostream>

namespace
{

std::string BoolToString(const bool value)
{
	return value ? "yes" : "no";
}

std::string SetToString(const std::set<Symbol>& s)
{
	if (s.empty())
	{
		return "-";
	}
	std::string res;
	for (const auto& sym : s)
	{
		res += (sym.IsEpsilon() ? "e" : sym.value) + ", ";
	}
	if (res.size() > 2)
	{
		res.erase(res.size() - 2);
	}
	return res;
}

} // namespace

TableGenerator::TableGenerator(std::vector<Production> p)
	: prods(std::move(p))
{
}

void TableGenerator::PrintTransformedGrammar() const
{
	std::cout << "\n--- Grammar after excluding left recursion ---\n";
	for (const auto& prod : prods)
	{
		std::cout << std::left << std::setw(15) << prod.lhs.value << " -> ";
		if (prod.rhs.empty() || (prod.rhs.size() == 1 && prod.rhs[0].IsEpsilon()))
		{
			std::cout << "e";
		}
		else
		{
			for (const auto& sym : prod.rhs)
			{
				std::cout << sym.value << " ";
			}
		}
		std::cout << std::endl;
	}
	std::cout << std::string(70, '-') << std::endl;
}

void TableGenerator::Generate()
{
	PrintTransformedGrammar();

	AnalysisEngine::FirstMap firsts;
	AnalysisEngine::FollowMap follows;
	AnalysisEngine::ComputeFirst(prods, firsts);
	AnalysisEngine::ComputeFollow(prods, firsts, follows, prods[0].lhs);

	for (auto& p : prods)
	{
		bool canBeEps = true;
		for (auto const& s : p.rhs)
		{
			if (s.type == SymbolType::Terminal || s.type == SymbolType::EndMarker)
			{
				p.guideSet.insert(s);
				canBeEps = false;
				break;
			}
			if (s.IsEpsilon())
			{
				break;
			}
			bool hasEps = false;
			for (auto const& f : firsts[s])
			{
				if (f.IsEpsilon())
				{
					hasEps = true;
				}
				else
				{
					p.guideSet.insert(f);
				}
			}
			if (!hasEps)
			{
				canBeEps = false;
				break;
			}
		}
		if (canBeEps)
		{
			for (auto const& f : follows[p.lhs])
			{
				p.guideSet.insert(f);
			}
		}
	}

	std::map<Symbol, std::vector<size_t>> ntToProdIndices;
	std::vector<Symbol> ntOrder;
	for (size_t i = 0; i < prods.size(); ++i)
	{
		if (!ntToProdIndices.contains(prods[i].lhs))
		{
			ntOrder.push_back(prods[i].lhs);
		}
		ntToProdIndices[prods[i].lhs].push_back(i);
	}

	int currentRow = 1;
	std::map<Symbol, int> ntTableEntry;
	int selectionRowsCount = static_cast<int>(prods.size());
	int rhsRowsStart = selectionRowsCount + 1;

	int tempRow = 1;
	for (const auto& nt : ntOrder)
	{
		ntTableEntry[nt] = tempRow;
		tempRow += static_cast<int>(ntToProdIndices[nt].size());
	}

	int currentRhsRowPointer = rhsRowsStart;
	for (const auto& nt : ntOrder)
	{
		const auto& indices = ntToProdIndices[nt];
		for (size_t i = 0; i < indices.size(); ++i)
		{
			size_t pIdx = indices[i];
			TableRow row;
			row.id = currentRow++;
			row.symbol = nt.value;
			row.guide = SetToString(prods[pIdx].guideSet);
			row.shift = false;
			row.error = (i == indices.size() - 1);
			row.pointer = std::to_string(currentRhsRowPointer);
			prods[pIdx].rhsStartRow = currentRhsRowPointer;

			if (prods[pIdx].rhs.size() == 1 && prods[pIdx].rhs[0].IsEpsilon())
			{
				currentRhsRowPointer += 1;
			}
			else
			{
				currentRhsRowPointer += static_cast<int>(prods[pIdx].rhs.size());
			}

			table.push_back(row);
		}
	}

	for (const auto& p : prods)
	{
		if (p.rhs.size() == 1 && p.rhs[0].IsEpsilon())
		{
			TableRow row;
			row.id = currentRow++;
			row.symbol = "e";
			row.guide = SetToString(p.guideSet);
			row.shift = false;
			row.error = true;
			row.pointer = "NULL";
			table.push_back(row);
			continue;
		}

		for (size_t i = 0; i < p.rhs.size(); ++i)
		{
			const auto& s = p.rhs[i];
			bool isLast = (i == p.rhs.size() - 1);
			TableRow row;
			row.id = currentRow++;
			row.symbol = s.value;

			if (s.type == SymbolType::Terminal || s.type == SymbolType::EndMarker)
			{
				row.guide = s.value;
				row.shift = true;
				row.error = true;
				row.pointer = isLast ? "NULL" : std::to_string(row.id + 1);
				if (s.type == SymbolType::EndMarker)
				{
					row.end = true;
				}
			}
			else
			{
				row.guide = SetToString(firsts[s]);
				row.shift = false;
				row.error = true;
				row.pointer = std::to_string(ntTableEntry[s]);
				row.stack = !isLast;
			}
			table.push_back(row);
		}
	}
	PrintResult();
}

void TableGenerator::PrintResult() const
{
	std::cout << "\n--- Result LL(1) table ---\n";
	std::cout << std::left << std::setw(6) << "№"
			  << std::setw(15) << "SYMBOL"
			  << std::setw(15) << "FOLLOW"
			  << std::setw(12) << "SHIFT"
			  << std::setw(12) << "ERROR"
			  << std::setw(12) << "PRT"
			  << std::setw(10) << "STACK"
			  << "END" << std::endl;
	std::cout << std::string(82, '-') << std::endl;

	for (const auto& row : table)
	{
		std::cout << std::left << std::setw(4) << row.id
				  << std::setw(15) << row.symbol
				  << std::setw(15) << row.guide
				  << std::setw(12) << BoolToString(row.shift)
				  << std::setw(12) << BoolToString(row.error)
				  << std::setw(10) << row.pointer
				  << std::setw(10) << BoolToString(row.stack)
				  << BoolToString(row.end) << std::endl;
	}
}