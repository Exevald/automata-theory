#include "Machines.h"

#include <fstream>
#include <ranges>

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
		m_transitions[std::make_pair(from, input)] = std::make_pair(to, output);
	}
}

MealyMachine MealyMachine::Minimize() const
{
	if (m_states.empty())
		return *this;

	std::set<std::string> inputs;
	for (const auto& [srcState, dstState] : m_transitions | std::views::keys)
	{
		inputs.insert(dstState);
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
				if (it1->second.second != it2->second.second)
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
					continue;

				for (const std::string& inp : inputs)
				{
					auto it1 = m_transitions.find(std::make_pair(s1, inp));
					auto it2 = m_transitions.find(std::make_pair(s2, inp));
					if (it1 == m_transitions.end() && it2 == m_transitions.end())
						continue;
					if (it1 == m_transitions.end() || it2 == m_transitions.end())
					{
						if (!distinguishable[key])
						{
							distinguishable[key] = true;
							changed = true;
						}
						break;
					}

					State next1 = it1->second.first;
					State next2 = it2->second.first;
					if (next1 == next2)
						continue;

					if (next1 > next2)
						std::swap(next1, next2);
					if (distinguishable.contains(std::make_pair(next1, next2)) && distinguishable[std::make_pair(next1, next2)])
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
					std::swap(a, b);
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

	for (const auto& [pair, outputPair] : m_transitions)
	{
		State from = pair.first;
		std::string input = pair.second;
		State to = outputPair.first;
		std::string out = outputPair.second;

		State newFrom = rep.at(from);
		State newTo = rep.at(to);
		minimized.m_transitions[std::make_pair(newFrom, input)] = std::make_pair(newTo, out);
	}

	return minimized;
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

				machine.m_transitions[std::make_pair(srcState, input)] = std::make_pair(dstState, output);
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
		oss << "  \"" << s.GetName() << "\" [label=\"" << s.GetName() << "\"];\n";
	}
	for (const auto& [fst, snd] : m_transitions)
	{
		const State& srcState = fst.first;
		const std::string& input = fst.second;
		const State& dstState = snd.first;
		const std::string& output = snd.second;
		oss << "  \"" << srcState.GetName() << "\" -> \"" << dstState.GetName() << "\" [label=\"" << input << "/" << output << "\"];\n";
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

std::set<State> MealyMachine::GetStates() const
{
	return m_states;
}

MealyTransitions MealyMachine::GetTransitions() const
{
	return m_transitions;
}

State MealyMachine::GetStartState() const
{
	return m_startState;
}
