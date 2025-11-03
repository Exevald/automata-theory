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

	for (const auto& [inputPair, outputSet] : mealy.GetTransitions())
	{
		if (outputSet.empty() || outputSet.size() > 1)
		{
			throw std::runtime_error("MealyMachine is not Deterministic (or malformed) for Moore conversion.");
		}

		const State& toState = outputSet.begin()->first;
		const std::string& output = outputSet.begin()->second;

		if (auto key = std::make_pair(toState, output); !mealyToMoore.contains(key))
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

	for (const auto& [inputPair, outputSet] : mealy.GetTransitions())
	{
		if (inputPair.first == mealyStart)
		{
			const std::string& input = inputPair.second;
			const State& toState = outputSet.begin()->first;
			const std::string& output = outputSet.begin()->second;
			State mooreTo = mealyToMoore.at(std::make_pair(toState, output));
			m_transitions[std::make_pair(m_startState, input)] = mooreTo;
		}
	}

	for (const auto& [inputPair, outputSet] : mealy.GetTransitions())
	{
		const State& fromMealy = inputPair.first;
		const std::string& input = inputPair.second;
		const State& toMealy = outputSet.begin()->first;
		const std::string& output = outputSet.begin()->second;

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

MooreMachine MooreMachine::FromDotFile(const std::string& filename)
{
    MooreMachine machine;
    std::ifstream file(filename);
    if (!file.is_open())
    {
       throw std::runtime_error("Cannot open file: " + filename);
    }

    std::string line;
    std::regex stateRegex(R"lit(^\s*"(\w+)"\s*\[label\s*=\s*"[^/]+/\s*([^"]+)"\]\s*$)lit");
    std::regex transitionRegex(R"lit(^\s*"(\w+)"\s*->\s*"(\w+)"\s*\[label\s*=\s*"([^"]+)"\]\s*$)lit");

    std::map<std::string, State> stateMap;

    while (std::getline(file, line))
    {
       std::smatch match;

       if (std::regex_match(line, match, stateRegex))
       {
          std::string name = match[1];
          std::string output = match[2];

          State state(name);
          stateMap[name] = state;
          machine.m_states.insert(state);
          machine.m_outputsMap[state] = output;

          if (machine.m_startState.GetName().empty())
          {
             machine.m_startState = state;
          }
       }
       else if (std::regex_match(line, match, transitionRegex))
       {
          std::string srcStateName = match[1];
          std::string dstStateName = match[2];
          std::string input = match[3];

          if (!stateMap.contains(srcStateName) || !stateMap.contains(dstStateName))
          {
             throw std::runtime_error("Transition references unknown state in DOT file.");
          }

          State srcState = stateMap.at(srcStateName);
          const State& dstState = stateMap.at(dstStateName);

          machine.m_transitions[std::make_pair(srcState, input)] = dstState;
       }
    }

    file.close();

    if (machine.m_startState.GetName().empty() && !machine.m_states.empty())
    {
        machine.m_startState = *machine.m_states.begin();
    }

    return machine;
}

MooreMachine MooreMachine::Minimize() const
{
	if (m_states.empty())
	{
		return *this;
	}

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
	for (const auto& [fst, snd] : m_transitions | std::views::keys)
	{
		inputs.insert(snd);
	}

	bool changed = true;
	while (changed)
	{
		changed = false;
		std::vector<std::set<State>> newPartitions;

		for (const auto& part : partitions)
		{
			std::map<std::vector<int>, std::set<State>> signatureGroups;

			for (const State& state : part)
			{
				std::vector<int> sig;
				for (const std::string& input : inputs)
				{
					if (auto it = m_transitions.find(std::make_pair(state, input));
						it != m_transitions.end())
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
				signatureGroups[sig].insert(state);
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

std::string MooreMachine::ToDotFile() const
{
	std::ostringstream oss;
	oss << "digraph MooreMachine {\n";

	for (const State& s : m_states)
	{
		std::string shape = (s == m_startState) ? ", shape=box, style=bold" : "";
		oss << "  \"" << s.GetName() << "\" [label=\"" << s.GetName() << " / " << m_outputsMap.at(s) << "\"" << shape << "];\n";
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
	file << ToDotFile();
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