#pragma once

#include <regex>
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