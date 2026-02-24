#include "Utils.h"

#include <iomanip>
#include <iostream>
#include <sstream>

bool GrammarUtils::IsNonTerminal(const Symbol& symbol)
{
	if (symbol.empty() || symbol == EPSILON)
	{
		return false;
	}
	return std::isupper(static_cast<unsigned char>(symbol[0])) || symbol.length() > 1;
}

bool GrammarUtils::IsTerminal(const Symbol& symbol)
{
	return !IsNonTerminal(symbol) && symbol != EPSILON;
}

bool GrammarUtils::IsEpsilonRule(const Production& production)
{
	return production.empty() || (production.size() == 1 && production[0] == EPSILON);
}

std::string GrammarUtils::ProductionToString(const Production& production)
{
	if (production.empty())
	{
		return EPSILON;
	}

	std::ostringstream oss;
	for (size_t i = 0; i < production.size(); ++i)
	{
		if (i > 0)
			oss << " ";
		oss << production[i];
	}
	return oss.str();
}

std::string GrammarUtils::ProductionsToString(const Productions& productions)
{
	std::ostringstream oss;
	for (size_t i = 0; i < productions.size(); ++i)
	{
		if (i > 0)
			oss << " | ";
		oss << ProductionToString(productions[i]);
	}
	return oss.str();
}

void GrammarUtils::PrintGrammar(const std::string& title, const Grammar& grammar)
{
	constexpr int BOX_WIDTH = 60;

	std::cout << "\n┌" << std::string(BOX_WIDTH - 2, '-') << "┐" << std::endl;
	std::cout << "│ " << std::left << std::setw(BOX_WIDTH - 4) << title << " │" << std::endl;
	std::cout << "├" << std::string(BOX_WIDTH - 2, '-') << "┤" << std::endl;

	for (const auto& [nonTerminal, productions] : grammar)
	{
		std::cout << "│ " << std::setw(4) << nonTerminal << " → "
				  << ProductionsToString(productions)
				  << std::setw(BOX_WIDTH - 12 - nonTerminal.length()) << ""
				  << " │" << std::endl;
	}

	std::cout << "└" << std::string(BOX_WIDTH - 2, '-') << "┘" << std::endl;
}
