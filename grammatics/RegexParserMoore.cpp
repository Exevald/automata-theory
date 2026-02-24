#include "Machines.h"
#include <cctype>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>

namespace
{

struct NFAState
{
	int id;
	static int counter;
	NFAState()
		: id(counter++)
	{
	}
	explicit NFAState(int i)
		: id(i)
	{
		if (i >= counter)
			counter = i + 1;
	}

	bool operator<(const NFAState& other) const
	{
		return id < other.id;
	}
};

int NFAState::counter = 0;

struct NFA
{
	std::set<NFAState> states;
	std::set<NFAState> finalStates;
	NFAState startState;
	std::map<std::pair<NFAState, std::string>, std::set<NFAState>> transitions;

	NFA()
		: startState(NFAState())
	{
		states.insert(startState);
	}

	NFA(NFAState start, NFAState final)
		: startState(start)
	{
		states.insert(start);
		states.insert(final);
		finalStates.insert(final);
	}
};

NFA ConcatNFA(NFA&& nfa1, NFA&& nfa2)
{
	NFA result;
	result.states = nfa1.states;
	result.states.insert(nfa2.states.begin(), nfa2.states.end());
	result.transitions = nfa1.transitions;
	for (const auto& [key, to_set] : nfa2.transitions)
	{
		result.transitions[key].insert(to_set.begin(), to_set.end());
	}
	for (const auto& final : nfa1.finalStates)
	{
		result.transitions[{ final, EPSILON }].insert(nfa2.startState);
	}
	result.startState = nfa1.startState;
	result.finalStates = nfa2.finalStates;
	return result;
}

NFA UnionNFA(NFA&& nfa1, NFA&& nfa2)
{
	NFA result;
	result.states = nfa1.states;
	result.states.insert(nfa2.states.begin(), nfa2.states.end());
	result.transitions = nfa1.transitions;
	for (const auto& [key, to_set] : nfa2.transitions)
	{
		result.transitions[key].insert(to_set.begin(), to_set.end());
	}

	NFAState newStart = NFAState();
	result.states.insert(newStart);
	result.startState = newStart;

	result.transitions[{ newStart, EPSILON }].insert(nfa1.startState);
	result.transitions[{ newStart, EPSILON }].insert(nfa2.startState);

	result.finalStates = nfa1.finalStates;
	result.finalStates.insert(nfa2.finalStates.begin(), nfa2.finalStates.end());

	return result;
}

NFA StarNFA(NFA&& nfa)
{
	NFA result;
	result.states = nfa.states;
	result.transitions = nfa.transitions;

	NFAState newStart = NFAState();
	NFAState newFinal = NFAState();

	result.states.insert(newStart);
	result.states.insert(newFinal);
	result.startState = newStart;
	result.finalStates.insert(newFinal);

	result.transitions[{ newStart, EPSILON }].insert(nfa.startState);
	for (const auto& old_final : nfa.finalStates)
	{
		result.transitions[{ old_final, EPSILON }].insert(newFinal);
	}
	for (const auto& old_final : nfa.finalStates)
	{
		result.transitions[{ old_final, EPSILON }].insert(nfa.startState);
	}
	result.transitions[{ newStart, EPSILON }].insert(newFinal);

	return result;
}

NFA PlusNFA(NFA&& nfa)
{
	NFA result;
	result.states = nfa.states;
	result.transitions = nfa.transitions;

	NFAState newStart = NFAState();
	NFAState newFinal = NFAState();

	result.states.insert(newStart);
	result.states.insert(newFinal);
	result.startState = newStart;
	result.finalStates.insert(newFinal);

	result.transitions[{ newStart, EPSILON }].insert(nfa.startState);
	for (const auto& old_final : nfa.finalStates)
	{
		result.transitions[{ old_final, EPSILON }].insert(newFinal);
	}
	for (const auto& old_final : nfa.finalStates)
	{
		result.transitions[{ old_final, EPSILON }].insert(nfa.startState);
	}

	return result;
}

MooreMachine nfaToMoore(const NFA& nfa)
{
	MooreMachine moore;
	std::map<NFAState, State> nfaToMooreStateMap;

	std::set<State> states;
	for (const auto& nfa_state : nfa.states)
	{
		nfaToMooreStateMap[nfa_state] = State("q" + std::to_string(nfa_state.id));
		states.insert(nfaToMooreStateMap[nfa_state]);
	}
	moore.SetStates(states);

	moore.SetStartState(nfaToMooreStateMap.at(nfa.startState));

	std::map<State, std::string> outputsMap;
	for (const auto& nfa_state : nfa.states)
	{
		State moore_state = nfaToMooreStateMap.at(nfa_state);
		if (nfa.finalStates.count(nfa_state))
		{
			outputsMap[moore_state] = "1";
		}
		else
		{
			outputsMap[moore_state] = "0";
		}
	}
	moore.SetOutputsMap(outputsMap);

	MooreTransitions transitions;
	for (const auto& [transition_key, to_states] : nfa.transitions)
	{
		const auto& [nfa_from, input] = transition_key;
		if (input == EPSILON)
			continue;

		State moore_from = nfaToMooreStateMap.at(nfa_from);
		for (const auto& nfa_to : to_states)
		{
			State moore_to = nfaToMooreStateMap.at(nfa_to);
			transitions[{ moore_from, input }] = moore_to;
		}
	}
	moore.SetTransitions(transitions);
	return moore;
}

} // namespace

class RegexParserMoore
{
public:
	explicit RegexParserMoore(std::string regex)
		: m_regex(std::move(regex))
		, m_pos(0)
	{
	}

	MooreMachine parse()
	{
		auto nfa = parseExpression();
		return nfaToMoore(nfa);
	}

private:
	NFA parseExpression()
	{
		auto lhs = parseTerm();
		while (m_pos < m_regex.size() && m_regex[m_pos] == '|')
		{
			m_pos++;
			auto rhs = parseTerm();
			lhs = UnionNFA(std::move(lhs), std::move(rhs));
		}
		return lhs;
	}

	NFA parseTerm()
	{
		NFA result;
		result.startState = NFAState();
		result.states.insert(result.startState);
		NFAState finalTemp = NFAState();
		result.finalStates.insert(finalTemp);
		result.states.insert(finalTemp);

		while (m_pos < m_regex.size() && m_regex[m_pos] != ')' && m_regex[m_pos] != '|')
		{
			auto factor = parseFactor();
			result = ConcatNFA(std::move(result), std::move(factor));
		}
		return result;
	}

	NFA parseFactor()
	{
		auto atom = parseAtom();
		while (m_pos < m_regex.size() && (m_regex[m_pos] == '*' || m_regex[m_pos] == '+'))
		{
			char op = m_regex[m_pos];
			m_pos++;
			if (op == '*')
			{
				atom = StarNFA(std::move(atom));
			}
			else if (op == '+')
			{
				atom = PlusNFA(std::move(atom));
			}
		}
		return atom;
	}

	NFA parseAtom()
	{
		if (m_pos >= m_regex.size())
		{
			throw std::runtime_error("Unexpected end of regex");
		}

		char c = m_regex[m_pos];
		if (c == '(')
		{
			m_pos++;
			auto expr = parseExpression();
			if (m_pos >= m_regex.size() || m_regex[m_pos] != ')')
			{
				throw std::runtime_error("Unmatched parenthesis");
			}
			m_pos++;
			return expr;
		}
		else if (c == ' ' || c == '\t')
		{
			m_pos++;
			return parseAtom();
		}
		else if (std::isalnum(c) || c == '_' || c == '0' || c == '1')
		{
			std::string symbol(1, c);
			m_pos++;
			NFAState start = NFAState();
			NFAState final = NFAState();
			NFA nfa(start, final);
			nfa.transitions[{ start, symbol }].insert(final);
			return nfa;
		}
		else
		{
			throw std::runtime_error("Invalid character in regex: " + std::string(1, c));
		}
	}

	std::string m_regex;
	size_t m_pos;
};

MooreMachine MooreMachine::FromRegex(const std::string& regex)
{
	std::string cleaned_regex;
	for (char c : regex)
	{
		if (c != ' ' && c != '\t')
		{
			cleaned_regex += c;
		}
	}
	RegexParserMoore parser(cleaned_regex);
	return parser.parse();
}

MealyMachine MooreMachine::ToDFA() const
{
	MealyMachine dfa;

	dfa.SetStates(this->GetStates());
	dfa.SetStartState(this->GetStartState());

	std::set<State> finalStates;
	for (const auto& [state, output] : this->GetOutputs())
	{
		if (output == "1")
		{
			finalStates.insert(state);
		}
	}
	dfa.SetFinalStates(finalStates);

	MealyTransitions dfa_transitions;
	for (const auto& [transition_key, to_state] : this->GetTransitions())
	{
		const State& from_state = transition_key.first;
		const std::string& input = transition_key.second;
		const State& to_moore_state = to_state;
		const std::string& output = this->GetOutputs().at(to_moore_state);
		dfa_transitions[{ from_state, input }].insert({ to_moore_state, output });
	}
	dfa.SetTransitions(dfa_transitions);

	return dfa;
}