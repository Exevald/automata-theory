#include "Machines.h"

#include <fstream>
#include <ranges>
#include <sstream>

namespace
{
State CreateNewState(int& counter)
{
	return State("Q" + std::to_string(counter++));
}
} // namespace

MealyMachine::MealyMachine()
	: m_startState("")
{
}

MealyMachine::MealyMachine(const MooreMachine& moore)
	: m_states(moore.GetStates())
	, m_startState(moore.GetStartState())
{
	for (const auto& [srcStateInfo, dstStateInfo] : moore.GetTransitions())
	{
		const State& from = srcStateInfo.first;
		const std::string& input = srcStateInfo.second;
		const State& to = dstStateInfo;
		std::string output = moore.GetOutputs().at(to);

		std::set<std::pair<State, std::string>> transitionSet;
		transitionSet.insert(std::make_pair(to, output));

		m_transitions[std::make_pair(from, input)] = transitionSet;

		if (output != "ε" && !output.empty())
		{
			m_finalStates.insert(from);
		}
	}
}

std::set<State> MealyMachine::EpsilonClosure(const std::set<State>& states) const
{
	std::set<State> closure = states;
	std::vector stack(states.begin(), states.end());

	while (!stack.empty())
	{
		State currentState = stack.back();
		stack.pop_back();

		if (m_transitions.contains({ currentState, EPSILON }))
		{
			for (const auto& nextPair : m_transitions.at({ currentState, EPSILON }))
			{
				const State& nextState = nextPair.first;
				if (!closure.contains(nextState))
				{
					closure.insert(nextState);
					stack.push_back(nextState);
				}
			}
		}
	}

	return closure;
}

MealyMachine MealyMachine::Determinize() const
{
	MealyMachine dfa;
	if (m_states.empty())
		return dfa;

	std::set<std::string> alphabet = GetAlphabet();

	std::set<State> initialClosure = EpsilonClosure({ m_startState });

	std::map<std::set<State>, State> dfaStatesMap;
	std::vector<std::set<State>> unmarkedStates;

	int stateCounter = 0;
	State newStartState = CreateNewState(stateCounter);

	dfaStatesMap[initialClosure] = newStartState;
	dfa.m_startState = newStartState;
	unmarkedStates.push_back(initialClosure);

	while (!unmarkedStates.empty())
	{
		std::set<State> currentNfaSet = unmarkedStates.back();
		unmarkedStates.pop_back();
		State currentDfaState = dfaStatesMap.at(currentNfaSet);
		dfa.m_states.insert(currentDfaState);

		bool isFinal = false;
		for (const State& nfaState : currentNfaSet)
		{
			if (m_finalStates.contains(nfaState))
			{
				isFinal = true;
				break;
			}
		}
		if (isFinal)
		{
			dfa.m_finalStates.insert(currentDfaState);
		}

		for (const std::string& input : alphabet)
		{
			std::set<State> nextNfaSet;
			std::set<std::string> possibleOutputs;

			for (const State& nfaState : currentNfaSet)
			{
				if (m_transitions.contains({ nfaState, input }))
				{
					for (const auto& [fst, snd] : m_transitions.at({ nfaState, input }))
					{
						nextNfaSet.insert(fst);
						possibleOutputs.insert(snd);
					}
				}
			}

			if (std::set<State> nextDfaSet = EpsilonClosure(nextNfaSet); !nextDfaSet.empty())
			{
				State nextDfaState;
				if (!dfaStatesMap.contains(nextDfaSet))
				{
					nextDfaState = CreateNewState(stateCounter);
					dfaStatesMap[nextDfaSet] = nextDfaState;
					unmarkedStates.push_back(nextDfaSet);
				}
				else
				{
					nextDfaState = dfaStatesMap.at(nextDfaSet);
				}

				std::string output = possibleOutputs.contains("1") ? "1" : "0";
				if (output == "0" && !possibleOutputs.empty())
				{
					output = *possibleOutputs.begin();
				}
				else if (possibleOutputs.empty())
				{
					output = "0";
				}

				dfa.m_transitions[{ currentDfaState, input }] = { { nextDfaState, output } };
			}
		}
	}

	return dfa.Minimize();
}

MealyMachine MealyMachine::Minimize() const
{
	if (m_states.empty())
	{
		return *this;
	}

	std::set<std::string> inputs;
	for (const auto& [fst, snd] : m_transitions | std::views::keys)
	{
		inputs.insert(snd);
	}

	std::map<std::pair<State, State>, bool> distinguishable;

	std::vector stateVec(m_states.begin(), m_states.end());
	for (size_t i = 0; i < stateVec.size(); ++i)
	{
		for (size_t j = i + 1; j < stateVec.size(); ++j)
		{
			const State& s1 = stateVec[i];
			const State& s2 = stateVec[j];
			bool diff = false;

			for (const std::string& inp : inputs)
			{
				auto it1 = m_transitions.find(std::make_pair(s1, inp));
				auto it2 = m_transitions.find(std::make_pair(s2, inp));

				if (it1 == m_transitions.end() && it2 == m_transitions.end())
					continue;

				if (it1 == m_transitions.end() || it2 == m_transitions.end())
				{
					diff = true;
					break;
				}

				const auto& [state1, out1] = *it1->second.begin();
				const auto& [state2, out2] = *it2->second.begin();
				if (out1 != out2)
				{
					diff = true;
					break;
				}
			}
			if (diff)
			{
				distinguishable[std::make_pair(s1, s2)] = true;
			}
		}
	}

	bool changed = true;
	while (changed)
	{
		changed = false;
		for (size_t i = 0; i < stateVec.size(); ++i)
		{
			for (size_t j = i + 1; j < stateVec.size(); ++j)
			{
				const State& s1 = stateVec[i];
				const State& s2 = stateVec[j];
				auto key = std::make_pair(s1, s2);

				if (distinguishable.contains(key) && distinguishable[key])
				{
					continue;
				}

				for (const std::string& inp : inputs)
				{
					auto it1 = m_transitions.find(std::make_pair(s1, inp));
					auto it2 = m_transitions.find(std::make_pair(s2, inp));

					if (it1 == m_transitions.end() && it2 == m_transitions.end())
					{
						continue;
					}

					if (it1 == m_transitions.end() || it2 == m_transitions.end())
					{
						if (!distinguishable[key])
						{
							distinguishable[key] = true;
							changed = true;
						}
						break;
					}

					State next1 = it1->second.begin()->first;
					State next2 = it2->second.begin()->first;

					if (next1 == next2)
					{
						continue;
					}

					if (next1 > next2)
					{
						std::swap(next1, next2);
					}

					if (distinguishable.contains(std::make_pair(next1, next2))
						&& distinguishable[std::make_pair(next1, next2)])
					{
						if (!distinguishable[key])
						{
							distinguishable[key] = true;
							changed = true;
						}
						break;
					}
				}
			}
		}
	}

	std::map<State, State> rep;
	std::vector<std::set<State>> classes;

	for (const State& s : m_states)
	{
		bool found = false;
		for (auto& cls : classes)
		{
			bool equiv = true;
			for (const State& other : cls)
			{
				State a = s, b = other;
				if (a > b)
				{
					std::swap(a, b);
				}
				if (a != b && distinguishable.contains(std::make_pair(a, b)) && distinguishable[std::make_pair(a, b)])
				{
					equiv = false;
					break;
				}
			}
			if (equiv)
			{
				cls.insert(s);
				rep[s] = *cls.begin();
				found = true;
				break;
			}
		}
		if (!found)
		{
			classes.push_back({ s });
			rep[s] = s;
		}
	}

	MealyMachine minimized;
	minimized.m_startState = rep.at(m_startState);

	for (const auto& cls : classes)
	{
		minimized.m_states.insert(*cls.begin());
	}

	for (const auto& [pair, outputSet] : m_transitions)
	{
		State from = pair.first;
		std::string input = pair.second;

		if (outputSet.empty())
		{
			continue;
		}

		const State& to = outputSet.begin()->first;
		const std::string& out = outputSet.begin()->second;

		State newFrom = rep.at(from);
		State newTo = rep.at(to);

		minimized.m_transitions[std::make_pair(newFrom, input)].insert(std::make_pair(newTo, out));
	}

	for (const State& s : m_finalStates)
	{
		if (rep.contains(s))
		{
			minimized.m_finalStates.insert(rep.at(s));
		}
	}

	return minimized;
}

std::set<std::string> MealyMachine::GetAlphabet() const
{
	std::set<std::string> alphabet;
	for (const auto& transition : m_transitions | std::views::keys)
	{
		if (transition.second != EPSILON)
		{
			alphabet.insert(transition.second);
		}
	}
	return alphabet;
}

MealyMachine MealyMachine::FromDotFile(const std::string& filename)
{
	MealyMachine machine;
	std::ifstream file(filename);
	if (!file.is_open())
	{
		throw std::runtime_error("Cannot open file: " + filename);
	}

	std::string line;
	std::regex stateRegex(R"lit(^\s*(\w+)\s*\[label\s*=\s*"([^"]*)"\]\s*$)lit");
	std::regex transitionRegex(R"lit(^\s*(\w+)\s*->\s*(\w+)\s*\[label\s*=\s*"([^"]*)"\]\s*$)lit");
	std::regex labelRegex("^([^/]+)/(.+)$");

	std::map<std::string, State> stateMap;

	while (std::getline(file, line))
	{

		if (std::smatch match; std::regex_match(line, match, stateRegex))
		{
			std::string name = match[1];
			stateMap[name] = State(name);
			machine.m_states.insert(State(name));
			if (machine.m_states.size() == 1)
			{
				machine.m_startState = State(name);
			}
		}
		else if (std::regex_match(line, match, transitionRegex))
		{
			std::string srcStateName = match[1];
			std::string dstStateName = match[2];
			std::string label = match[3];

			if (std::smatch labelMatch; std::regex_match(label, labelMatch, labelRegex))
			{
				std::string input = labelMatch[1];
				std::string output = labelMatch[2];

				State srcState = stateMap[srcStateName];
				State dstState = stateMap[dstStateName];

				machine.m_transitions[std::make_pair(srcState, input)].insert(
					std::make_pair(dstState, output));
			}
			else
			{
				throw std::runtime_error("Invalid transition label format: " + label);
			}
		}
	}

	file.close();
	return machine;
}

std::string MealyMachine::ToDotFile() const
{
	std::ostringstream oss;
	oss << "digraph MealyMachine {\n";

	for (const State& s : m_states)
	{
		std::string attributes;

		if (s == m_startState)
		{
			attributes += "style=bold, shape=box";
		}
		if (m_finalStates.contains(s))
		{
			if (!attributes.empty())
			{
				attributes += ", ";
			}
			attributes += "shape=doublecircle";
		}
		if (!attributes.empty())
		{
			attributes = ", " + attributes;
		}

		oss << "  \"" << s.GetName() << "\" [label=\"" << s.GetName() << "\"" << attributes << "];\n";
	}

	for (const auto& [fst, outputSet] : m_transitions)
	{
		const State& srcState = fst.first;
		const std::string& input = fst.second;

		for (const auto& [state, out] : outputSet)
		{
			const State& dstState = state;
			const std::string& output = out;
			oss << "  \"" << srcState.GetName() << "\" -> \"" << dstState.GetName() << "\" [label=\"" << input << "/" << output << "\"];\n";
		}
	}
	oss << "}\n";

	return oss.str();
}

void MealyMachine::SaveToFile(const std::string& filename) const
{
	std::ofstream file(filename);
	if (!file.is_open())
	{
		throw std::runtime_error("Cannot open file for writing: " + filename);
	}
	file << ToDotFile();
	file.close();
}