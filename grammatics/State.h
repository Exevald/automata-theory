#pragma once

#include <map>
#include <regex>
#include <set>
#include <string>
#include <utility>

class State
{
public:
	State() = default;
	explicit State(std::string name)
		: m_name(std::move(name))
	{
	}

	bool operator==(const State& other) const
	{
		return m_name == other.m_name;
	}
	bool operator<(const State& other) const
	{
		return m_name < other.m_name;
	}
	[[nodiscard]] std::string GetName() const
	{
		return m_name;
	}
	auto operator<=>(const State& state) const = default;

private:
	std::string m_name;
};

using MealyTransitions = std::map<std::pair<State, std::string>, std::set<std::pair<State, std::string>>>;
using MooreTransitions = std::map<std::pair<State, std::string>, State>;

const std::string EPSILON = "ε";
using DeterministicMealyTarget = std::pair<State, std::string>;