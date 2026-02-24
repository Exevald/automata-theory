#include "CYKParser.h"

using ParseTable = std::vector<std::vector<SymbolSet>>;

namespace
{

void InitializeTerminals(
	ParseTable& table,
	const Grammar& grammar,
	const std::string& input)
{
	const int n = static_cast<int>(input.length());

	for (int i = 0; i < n; ++i)
	{
		const Symbol terminal(1, input[i]);

		for (const auto& [lhs, productions] : grammar)
		{
			for (const auto& production : productions)
			{
				if (production.size() == 1 && production[0] == terminal)
				{
					table[i][0].insert(lhs);
				}
			}
		}
	}
}

void FillTable(
	ParseTable& table,
	const Grammar& grammar,
	const std::string& input)
{
	const int n = static_cast<int>(input.length());

	for (int len = 2; len <= n; ++len)
	{
		for (int i = 0; i <= n - len; ++i)
		{
			const int j = len - 1;

			for (int k = 1; k < len; ++k)
			{
				const SymbolSet& leftSet = table[i][k - 1];
				const SymbolSet& rightSet = table[i + k][len - k - 1];

				for (const auto& [lhs, productions] : grammar)
				{
					for (const auto& production : productions)
					{
						if (production.size() == 2)
						{
							const Symbol& B = production[0];
							const Symbol& C = production[1];

							if (leftSet.count(B) && rightSet.count(C))
							{
								table[i][j].insert(lhs);
							}
						}
					}
				}
			}
		}
	}
}

bool CheckStartSymbol(const ParseTable& table)
{
	if (table.empty() || table[0].empty())
	{
		return false;
	}

	const int n = static_cast<int>(table.size());
	const SymbolSet& result = table[0][n - 1];

	return result.count(START_SYMBOL) > 0 || result.count("S'") > 0;
}
} // namespace

bool CYKParser::ParseWithTable(
	const Grammar& grammar,
	const std::string& input)
{
	const int n = static_cast<int>(input.length());
	if (n == 0)
	{
		return false;
	}
	ParseTable table(n, std::vector<SymbolSet>(n));
	InitializeTerminals(table, grammar, input);
	FillTable(table, grammar, input);
	return CheckStartSymbol(table);
}

bool CYKParser::Parse(const Grammar& grammar, const std::string& input)
{
	return ParseWithTable(grammar, input);
}
