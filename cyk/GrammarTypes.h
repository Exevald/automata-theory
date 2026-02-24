#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

inline const std::string EPSILON = "ε";
inline const std::string START_SYMBOL = "S";

using Symbol = std::string;
using Production = std::vector<Symbol>;
using Productions = std::vector<Production>;
using Grammar = std::map<Symbol, Productions>;
using SymbolSet = std::set<Symbol>;
using SymbolVector = std::vector<Symbol>;
