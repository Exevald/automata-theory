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

	[[nodiscard]] std::set<State> GetStates() const { return m_states; }
	[[nodiscard]] MealyTransitions GetTransitions() const { return m_transitions; }
	[[nodiscard]] State GetStartState() const { return m_startState; }
	[[nodiscard]] std::set<State> GetFinalStates() const { return m_finalStates; }
	[[nodiscard]] std::set<std::string> GetAlphabet() const;

	void SetStates(const std::set<State>& states) { m_states = states; }
	void SetTransitions(const MealyTransitions& transitions) { m_transitions = transitions; }
	void SetStartState(const State& startState) { m_startState = startState; }
	void SetFinalStates(const std::set<State>& finalStates) { m_finalStates = finalStates; }

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
	friend class Grammar;

public:
	MooreMachine();
	explicit MooreMachine(const MealyMachine& mealy);

	[[nodiscard]] MooreMachine Minimize() const;

	static MooreMachine FromDotFile(const std::string& filename);
	[[nodiscard]] std::string ToDotFile() const;
	void SaveToFile(const std::string& filename) const;

	[[nodiscard]] std::set<State> GetStates() const { return m_states; }
	[[nodiscard]] std::map<State, std::string> GetOutputs() const { return m_outputsMap; }
	[[nodiscard]] MooreTransitions GetTransitions() const { return m_transitions; }
	[[nodiscard]] State GetStartState() const { return m_startState; }

	static MooreMachine FromRegex(const std::string& regex);
	[[nodiscard]] MealyMachine ToDFA() const;

	void SetStates(const std::set<State>& states) { m_states = states; }
	void SetOutputsMap(const std::map<State, std::string>& outputsMap) { m_outputsMap = outputsMap; }
	void SetTransitions(const MooreTransitions& transitions) { m_transitions = transitions; }
	void SetStartState(const State& startState) { m_startState = startState; }

private:
	std::set<State> m_states;
	std::map<State, std::string> m_outputsMap;
	MooreTransitions m_transitions;
	State m_startState;
};