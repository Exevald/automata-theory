#pragma once

#include "GrammarTypes.h"

namespace GrammarUtils
{
bool IsNonTerminal(const Symbol& symbol);
bool IsTerminal(const Symbol& symbol);
bool IsEpsilonRule(const Production& production);
std::string ProductionToString(const Production& production);
std::string ProductionsToString(const Productions& productions);
void PrintGrammar(const std::string& title, const Grammar& grammar);
}; // namespace GrammarUtils
