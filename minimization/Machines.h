#pragma once

#include "State.h"

#include <map>
#include <set>
#include <string>

class MealyMachine;
class MooreMachine;

using MealyTransitions = std::map<std::pair<State, std::string>, std::pair<State, std::string>>;
using MooreTransitions = std::map<std::pair<State, std::string>, State>;

class MealyMachine
{
public:
	MealyMachine();
	explicit MealyMachine(const MooreMachine& moore);
	[[nodiscard]] MealyMachine Minimize() const;
	static MealyMachine FromDotFile(const std::string& filename);
	[[nodiscard]] std::string ToDotFile() const;
	void SaveToFile(const std::string& filename) const;
	[[nodiscard]] std::set<State> GetStates() const;
	[[nodiscard]] MealyTransitions GetTransitions() const;
	[[nodiscard]] State GetStartState() const;

private:
	std::set<State> m_states;
	MealyTransitions m_transitions;
	State m_startState;
};

class MooreMachine
{
public:
	MooreMachine();
	explicit MooreMachine(const MealyMachine& mealy);
	[[nodiscard]] MooreMachine Minimize() const;
	[[nodiscard]] std::string ToDot() const;
	void SaveToFile(const std::string& filename) const;
	[[nodiscard]] std::set<State> GetStates() const;
	[[nodiscard]] std::map<State, std::string> GetOutputs() const;
	[[nodiscard]] MooreTransitions GetTransitions() const;
	[[nodiscard]] State GetStartState() const;

private:
	std::set<State> m_states;
	std::map<State, std::string> m_outputsMap;
	MooreTransitions m_transitions;
	State m_startState;
};