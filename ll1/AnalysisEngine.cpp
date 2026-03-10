#include "AnalysisEngine.h"
#include "GrammarProcessor.h"

void AnalysisEngine::ComputeFirst(const std::vector<Production>& prods, FirstMap& firsts)
{
	for (const auto& p : prods)
	{
		firsts[p.lhs];
		for (const auto& s : p.rhs)
		{
			if (s.type == SymbolType::NonTerminal)
			{
				firsts[s];
			}
		}
	}

	bool changed = true;
	while (changed)
	{
		changed = false;
		for (const auto& p : prods)
		{
			auto& target = firsts[p.lhs];
			const size_t oldSize = target.size();
			bool allEps = true;
			for (const auto& s : p.rhs)
			{
				if (s.type == SymbolType::Terminal || s.type == SymbolType::EndMarker)
				{
					target.insert(s);
					allEps = false;
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
						target.insert(f);
					}
				}
				if (!hasEps)
				{
					allEps = false;
					break;
				}
			}
			if (allEps)
			{
				target.insert(GrammarProcessor::CreateSymbol("e"));
			}
			if (target.size() > oldSize)
			{
				changed = true;
			}
		}
	}
}

void AnalysisEngine::ComputeFollow(const std::vector<Production>& prods, const FirstMap& firsts, FollowMap& follows, const Symbol& start)
{
	follows[start].insert(GrammarProcessor::CreateSymbol("⊥"));
	bool changed = true;
	while (changed)
	{
		changed = false;
		for (const auto& p : prods)
		{
			for (size_t i = 0; i < p.rhs.size(); ++i)
			{
				if (p.rhs[i].type != SymbolType::NonTerminal)
				{
					continue;
				}
				auto& currentFollow = follows[p.rhs[i]];
				const size_t oldSize = currentFollow.size();
				bool trailerEps = true;
				for (size_t j = i + 1; j < p.rhs.size(); ++j)
				{
					const auto& next = p.rhs[j];
					if (next.type == SymbolType::Terminal || next.type == SymbolType::EndMarker)
					{
						currentFollow.insert(next);
						trailerEps = false;
						break;
					}
					if (next.IsEpsilon())
					{
						continue;
					}
					bool hasEps = false;
					for (auto const& f : firsts.at(next))
					{
						if (f.IsEpsilon())
						{
							hasEps = true;
						}
						else
						{
							currentFollow.insert(f);
						}
					}
					if (!hasEps)
					{
						trailerEps = false;
						break;
					}
				}
				if (trailerEps)
				{
					for (auto const& f : follows[p.lhs])
					{
						currentFollow.insert(f);
					}
				}
				if (currentFollow.size() > oldSize)
				{
					changed = true;
				}
			}
		}
	}
}