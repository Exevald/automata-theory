#include "Machines.h"

#include <fstream>

MooreMachine::MooreMachine()
	: m_startState("")
{
}

MooreMachine::MooreMachine(const MealyMachine& mealy)
	: m_startState(mealy.GetStartState())
{
	std::map<std::pair<State, std::string>, State> stateOutputToNewState;

	for (const auto& [srcStateInfo, dstStateInfo] : mealy.GetTransitions())
	{
		const State& dstState = dstStateInfo.first;
		const std::string& output = dstStateInfo.second;

		std::string newStateName = dstState.GetName() + "_" + output;
		State newState(newStateName);

		stateOutputToNewState[std::make_pair(dstState, output)] = newState;
		m_outputs[newState] = output;
		m_states.insert(newState);
	}

	bool foundStart = false;
	for (const auto& [srcStateInfo, dstStateInfo] : mealy.GetTransitions())
	{
		if (srcStateInfo.first.GetName() == mealy.GetStartState().GetName())
		{
			const std::string& output = dstStateInfo.second;
			m_startState = State(mealy.GetStartState().GetName() + "_" + output);
			foundStart = true;
			break;
		}
	}

	if (!foundStart && !mealy.GetTransitions().empty())
	{
		auto it = mealy.GetTransitions().begin();
		const std::string& output = it->second.second;
		m_startState = State(mealy.GetStartState().GetName() + "_" + output);
	}
	else if (mealy.GetTransitions().empty())
	{
		m_startState = State(mealy.GetStartState().GetName() + "_ε");
		m_states.insert(m_startState);
		m_outputs[m_startState] = "ε";
	}

	for (const auto& [srcStateInfo, dstStateInfo] : mealy.GetTransitions())
	{
		const State& srcState = srcStateInfo.first;
		const std::string& input = srcStateInfo.second;
		const State& dstState = dstStateInfo.first;
		const std::string& output = dstStateInfo.second;

		State fromMooreState = m_startState;
		bool foundFromMooreState = false;

		for (const auto& [srcStateInfo, dstStateInfo] : stateOutputToNewState)
		{
			if (srcStateInfo.first.GetName() == srcState.GetName())
			{
				fromMooreState = dstStateInfo;
				foundFromMooreState = true;
				break;
			}
		}
		if (!foundFromMooreState)
		{
			for (const auto& [srcStateName, dstStateName] : mealy.GetTransitions())
			{
				if (srcStateName.first.GetName() == srcState.GetName())
				{
					fromMooreState = State(srcState.GetName() + "_" + dstStateName.second);
					break;
				}
			}
		}
		State toMooreState = stateOutputToNewState[std::make_pair(dstState, output)];
		m_transitions[std::make_pair(fromMooreState, input)] = toMooreState;
	}
}

std::string MooreMachine::ToDot() const
{
	std::ostringstream oss;
	oss << "digraph MooreMachine {\n";

	for (const State& s : m_states)
	{
		oss << "  " << s.GetName() << " [label=\"" << s.GetName() << " / " << m_outputs.at(s) << "\"];\n";
	}

	for (const auto& [srcState, dstState] : m_transitions)
	{
		const State& from = srcState.first;
		const std::string& input = srcState.second;
		const State& to = dstState;
		oss << "  " << from.GetName() << " -> " << to.GetName() << " [label=\"" << input << "\"];\n";
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
	return m_outputs;
}

MooreTransitions MooreMachine::GetTransitions() const
{
	return m_transitions;
}

State MooreMachine::GetStartState() const
{
	return m_startState;
}