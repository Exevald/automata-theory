#pragma once

#include "State.h"
#include <map>
#include <set>
#include <string>

class MooreMachine;

class MealyMachine
{
	friend class MooreMachine;
	friend class Grammar;

public:
	MealyMachine();
	explicit MealyMachine(const MooreMachine& moore);

	[[nodiscard]] MealyMachine Determinize() const;
	[[nodiscard]] MealyMachine Minimize() const;

	static MealyMachine FromDotFile(const std::string& filename);
	[[nodiscard]] std::string ToDotFile() const;
	void SaveToFile(const std::string& filename) const;

	[[nodiscard]] std::set<State> GetStates() const;
	[[nodiscard]] MealyTransitions GetTransitions() const;
	[[nodiscard]] State GetStartState() const;
	[[nodiscard]] std::set<State> GetFinalStates() const;
	[[nodiscard]] std::set<std::string> GetAlphabet() const;

private:
	[[nodiscard]] std::set<State> EpsilonClosure(const std::set<State>& states) const;

	std::set<State> m_states;
	MealyTransitions m_transitions;
	State m_startState;
	std::set<State> m_finalStates;
};

class MooreMachine
{
	friend class MealyMachine;

public:
	MooreMachine();
	explicit MooreMachine(const MealyMachine& mealy);

	[[nodiscard]] MooreMachine Minimize() const;

	static MooreMachine FromDotFile(const std::string& filename);
	[[nodiscard]] std::string ToDotFile() const;
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