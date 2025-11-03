#include "Grammar.h"
#include <cctype>
#include <fstream>
#include <ranges>
#include <sstream>

Grammar Grammar::FromString(const std::string& grammarText)
{
	Grammar grammar;
	std::istringstream stream(grammarText);
	std::string line;
	std::regex ruleRegex(R"lit(\s*([A-Z])\s*->\s*([^|\s]+)(.*))lit");
	std::regex altRegex(R"lit(\|\s*([^|\s]+))lit");

	while (std::getline(stream, line))
	{
		if (line.empty() || line.find("//") == 0)
		{
			continue;
		}

		if (std::smatch match; std::regex_match(line, match, ruleRegex))
		{
			std::string nonTerminal = match[1].str();
			grammar.m_nonTerminals.insert(nonTerminal);

			if (grammar.m_startSymbol.empty())
			{
				grammar.m_startSymbol = nonTerminal;
			}

			std::string firstRHS = match[2].str();
			grammar.m_rules.emplace_back(nonTerminal, firstRHS);

			std::string alternatives = match[3].str();
			auto searchStart(alternatives.cbegin());

			while (std::regex_search(searchStart, alternatives.cend(), match, altRegex))
			{
				std::string altRHS = match[1].str();
				grammar.m_rules.emplace_back(nonTerminal, altRHS);
				searchStart = match.suffix().first;
			}
		}
	}

	grammar.DetermineType();
	return grammar;
}

Grammar Grammar::FromFile(const std::string& filename)
{
	std::ifstream file(filename);
	if (!file.is_open())
	{
		throw std::runtime_error("Cannot open grammar file: " + filename);
	}

	std::stringstream buffer;
	buffer << file.rdbuf();

	return FromString(buffer.str());
}

void Grammar::DetermineType()
{
	bool isRightLinear = true;
	bool isLeftLinear = true;

	const std::regex rightGrammaticsReg(R"lit(^[a-z]+([A-Z])?|ε$)lit");
	const std::regex leftGrammaticsReg(R"lit(^([A-Z])[a-z]+|ε$)lit");

	for (const auto& rule : m_rules | std::views::values)
	{
		const std::string& rhs = rule;
		if (!std::regex_match(rhs, rightGrammaticsReg))
		{
			isRightLinear = false;
		}
		if (!std::regex_match(rhs, leftGrammaticsReg))
		{
			isLeftLinear = false;
		}
	}

	if (isRightLinear && !isLeftLinear)
	{
		m_type = GrammarType::RightLinear;
	}
	else if (isLeftLinear && !isRightLinear)
	{
		m_type = GrammarType::LeftLinear;
	}
	else
	{
		m_type = GrammarType::Unknown;
	}
}

MealyMachine Grammar::ToNFA() const
{
	MealyMachine nfa;
	if (m_startSymbol.empty() || m_type == GrammarType::Unknown)
	{
		return nfa;
	}

	nfa.m_startState = State(m_startSymbol);
	for (const auto& nt : m_nonTerminals)
	{
		nfa.m_states.insert(State(nt));
	}

	State finalState("Z");
	nfa.m_states.insert(finalState);

	if (m_nonTerminals.contains(finalState.GetName()))
	{
		finalState = State("Z_final");
		nfa.m_states.insert(finalState);
	}

	if (m_type == GrammarType::RightLinear)
	{
		nfa.m_finalStates.insert(finalState);
		for (const auto& [left, right] : m_rules)
		{
			const State fromState(left);

			if (const std::string& rhs = right; rhs == "ε")
			{
				nfa.m_transitions[{ fromState, EPSILON }].insert({ finalState, "1" });
			}
			else if (rhs.length() == 1 && std::islower(rhs[0]))
			{
				std::string input(1, rhs[0]);
				nfa.m_transitions[{ fromState, input }].insert({ finalState, "1" });
			}
			else if (rhs.length() == 2 && std::islower(rhs[0]) && std::isupper(rhs[1])) // A -> aB
			{
				std::string input(1, rhs[0]);
				State toState(rhs.substr(1, 1));
				nfa.m_transitions[{ fromState, input }].insert({ toState, "0" });
			}
		}
	}
	else if (m_type == GrammarType::LeftLinear)
	{
		State newStart("S0");
		nfa.m_states.insert(newStart);
		nfa.m_startState = newStart;
		nfa.m_finalStates.insert(finalState);

		for (const auto& [left, right] : m_rules)
		{
			const State toState(left);

			if (const std::string& rhs = right; rhs == "ε")
			{
				nfa.m_transitions[{ newStart, EPSILON }].insert({ toState, "1" });
			}
			else if (rhs.length() == 1 && std::islower(rhs[0]))
			{
				std::string input(1, rhs[0]);
				nfa.m_transitions[{ newStart, input }].insert({ toState, "1" });
			}
			else if (rhs.length() == 2 && std::isupper(rhs[0]) && std::islower(rhs[1]))
			{
				std::string input(1, rhs[1]);
				State fromState(rhs.substr(0, 1));
				nfa.m_transitions[{ fromState, input }].insert({ toState, "0" });
			}
		}
	}

	return nfa;
}