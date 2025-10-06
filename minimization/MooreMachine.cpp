#include "Machines.h"

#include <fstream>
#include <ranges>

MooreMachine::MooreMachine()
	: m_startState("")
{
}

MooreMachine::MooreMachine(const MealyMachine& mealy)
{
	std::map<std::pair<State, std::string>, State> mealyToMoore;

	for (const auto& [inputPair, outputPair] : mealy.GetTransitions())
	{
		const State& toState = outputPair.first;
		const std::string& output = outputPair.second;
		auto key = std::make_pair(toState, output);
		if (mealyToMoore.find(key) == mealyToMoore.end())
		{
			std::string newName = toState.GetName() + "/" + output;
			mealyToMoore[key] = State(newName);
			m_states.insert(mealyToMoore[key]);
			m_outputsMap[mealyToMoore[key]] = output;
		}
	}

	m_startState = State("S0");
	m_states.insert(m_startState);
	m_outputsMap[m_startState] = "ε";
	const State& mealyStart = mealy.GetStartState();
	for (const auto& [inputPair, outputPair] : mealy.GetTransitions())
	{
		if (inputPair.first == mealyStart)
		{
			const std::string& input = inputPair.second;
			const State& toState = outputPair.first;
			const std::string& output = outputPair.second;
			State mooreTo = mealyToMoore.at(std::make_pair(toState, output));
			m_transitions[std::make_pair(m_startState, input)] = mooreTo;
		}
	}

	for (const auto& [inputPair, outputPair] : mealy.GetTransitions())
	{
		const State& fromMealy = inputPair.first;
		const std::string& input = inputPair.second;
		const State& toMealy = outputPair.first;
		const std::string& output = outputPair.second;

		for (const auto& [key, mooreFrom] : mealyToMoore)
		{
			if (key.first == fromMealy)
			{
				State mooreTo = mealyToMoore.at(std::make_pair(toMealy, output));
				m_transitions[std::make_pair(mooreFrom, input)] = mooreTo;
			}
		}
	}
}

MooreMachine MooreMachine::Minimize() const
{
	if (m_states.empty())
		return *this;

	std::map<std::string, std::set<State>> outputGroups;
	for (const State& s : m_states)
	{
		outputGroups[m_outputsMap.at(s)].insert(s);
	}

	std::vector<std::set<State>> partitions;
	for (const auto& states : outputGroups | std::views::values)
	{
		partitions.push_back(states);
	}

	std::set<std::string> inputs;
	for (const auto& pair : m_transitions | std::views::keys)
	{
		inputs.insert(pair.second);
	}

	bool changed = true;
	while (changed)
	{
		changed = false;
		std::vector<std::set<State>> newPartitions;

		for (const auto& part : partitions)
		{
			std::map<std::vector<int>, std::set<State>> signatureGroups;

			for (const State& s : part)
			{
				std::vector<int> sig;
				for (const std::string& inp : inputs)
				{
					auto it = m_transitions.find(std::make_pair(s, inp));
					if (it != m_transitions.end())
					{
						const State& next = it->second;
						int classIdx = -1;
						for (int i = 0; i < partitions.size(); ++i)
						{
							if (partitions[i].contains(next))
							{
								classIdx = i;
								break;
							}
						}
						sig.push_back(classIdx);
					}
					else
					{
						sig.push_back(-1);
					}
				}
				signatureGroups[sig].insert(s);
			}

			if (signatureGroups.size() > 1)
			{
				changed = true;
				for (const auto& group : signatureGroups | std::views::values)
				{
					newPartitions.push_back(group);
				}
			}
			else
			{
				newPartitions.push_back(part);
			}
		}

		partitions = std::move(newPartitions);
	}

	MooreMachine minimized;
	std::map<State, State> stateMap;

	for (const auto& part : partitions)
	{
		State rep = *part.begin();
		for (const State& s : part)
		{
			stateMap[s] = rep;
		}
	}

	for (const auto& part : partitions)
	{
		State rep = *part.begin();
		minimized.m_states.insert(rep);
		minimized.m_outputsMap[rep] = m_outputsMap.at(rep);
	}

	minimized.m_startState = stateMap.at(m_startState);

	for (const auto& [pair, to] : m_transitions)
	{
		State from = pair.first;
		std::string input = pair.second;
		State newFrom = stateMap.at(from);
		const State& newTo = stateMap.at(to);
		minimized.m_transitions[std::make_pair(newFrom, input)] = newTo;
	}

	return minimized;
}

std::string MooreMachine::ToDot() const
{
	std::ostringstream oss;
	oss << "digraph MooreMachine {\n";

	for (const State& s : m_states)
	{
		oss << "  \"" << s.GetName() << "\" [label=\"" << s.GetName() << " / " << m_outputsMap.at(s) << "\"];\n";
	}

	for (const auto& [srcState, dstState] : m_transitions)
	{
		const State& from = srcState.first;
		const std::string& input = srcState.second;
		const State& to = dstState;
		oss << "  \"" << from.GetName() << "\" -> \"" << to.GetName() << "\" [label=\"" << input << "\"];\n";
	}
	oss << "}\n";

	return oss.str();
}

void MooreMachine::SaveToFile(const std::string& filename) const
{
	std::ofstream file(filename);
	if (!file.is_open())
	{
		throw std::runtime_error("Cannot open file for writing: " + filename);
	}
	file << ToDot();
	file.close();
}

std::set<State> MooreMachine::GetStates() const
{
	return m_states;
}

std::map<State, std::string> MooreMachine::GetOutputs() const
{
	return m_outputsMap;
}

MooreTransitions MooreMachine::GetTransitions() const
{
	return m_transitions;
}

State MooreMachine::GetStartState() const
{
	return m_startState;
}