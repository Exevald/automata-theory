#pragma once

#include "Common.h"

#include <map>

namespace AnalysisEngine
{
using FirstMap = std::map<Symbol, std::set<Symbol>>;
using FollowMap = std::map<Symbol, std::set<Symbol>>;

void ComputeFirst(const std::vector<Production>& prods, FirstMap& firsts);
void ComputeFollow(
	const std::vector<Production>& prods,
	const FirstMap& firsts,
	FollowMap& follows,
	const Symbol& start);

} // namespace AnalysisEngine