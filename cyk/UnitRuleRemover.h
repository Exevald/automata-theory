#pragma once

#include "GrammarTypes.h"

namespace UnitRuleRemover
{
Grammar RemoveUnitRules(Grammar grammar);

bool IsUnitRule(const Production& production, const SymbolSet& nonTerminals);
SymbolSet ExtractNonTerminals(const Grammar& grammar);
}; // namespace UnitRuleRemover
