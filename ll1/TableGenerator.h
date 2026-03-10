#pragma once

#include "Common.h"

#include <iomanip>
#include <string>

struct TableRow
{
	int id{};
	std::string symbol;
	std::string guide;
	bool shift = false;
	bool error = false;
	std::string pointer = "NULL";
	bool stack = false;
	bool end = false;
};

class TableGenerator
{
	std::vector<Production> prods;
	std::vector<TableRow> table;

public:
	explicit TableGenerator(std::vector<Production> p);

	void PrintTransformedGrammar() const;

	void Generate();

	void PrintResult() const;
};