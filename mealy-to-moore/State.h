#pragma once

#include <regex>
#include <string>

class State
{
public:
	State() = default;
	explicit State(const std::string& name)
		: m_name(name)
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
	std::string GetName() const
	{
		return m_name;
	}

private:
	std::string m_name;
};