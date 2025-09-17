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
	static MealyMachine FromDotFile(const std::string& filename);
	std::string ToDotFile() const;
	void SaveToFile(const std::string& filename) const;
	std::set<State> GetStates() const;
	MealyTransitions GetTransitions() const;
	State GetStartState() const;

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
	std::string ToDot() const;
	void SaveToFile(const std::string& filename) const;
	std::set<State> GetStates() const;
	std::map<State, std::string> GetOutputs() const;
	MooreTransitions GetTransitions() const;
	State GetStartState() const;

private:
	std::set<State> m_states;
	std::map<State, std::string> m_outputs;
	MooreTransitions m_transitions;
	State m_startState;
};