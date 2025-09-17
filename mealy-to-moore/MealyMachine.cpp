#include "Machines.h"

#include <fstream>

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

MealyMachine MealyMachine::FromDotFile(const std::string& filename)
{
	MealyMachine machine;
	std::ifstream file(filename);
	if (!file.is_open())
	{
		throw std::runtime_error("Cannot open file: " + filename);
	}

	std::string line;
	std::regex stateRegex("^\\s*(\\w+)\\s*\\[label\\s*=\\s*\"([^\"]*)\"\\]\\s*$");
	std::regex transitionRegex("^\\s*(\\w+)\\s*->\\s*(\\w+)\\s*\\[label\\s*=\\s*\"([^\"]*)\"\\]\\s*$");
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
		oss << "  " << s.GetName() << " [label=\"" << s.GetName() << "\"];\n";
	}
	for (const auto& [fst, snd] : m_transitions)
	{
		const State& srcState = fst.first;
		const std::string& input = fst.second;
		const State& dstState = snd.first;
		const std::string& output = snd.second;
		oss << "  " << srcState.GetName() << " -> " << dstState.GetName() << " [label=\"" << input << "/" << output << "\"];\n";
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
