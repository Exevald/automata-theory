#pragma once

#include "GrammarTypes.h"

#include <string>

struct TestCase
{
	int id;
	std::string name;
	std::string description;
	Grammar grammar;
	std::string input;
	bool expected;

	TestCase(
		int testId,
		std::string testName,
		std::string testDescription,
		Grammar testGrammar,
		std::string testInput,
		bool testExpected)
		: id(testId)
		, name(std::move(testName))
		, description(std::move(testDescription))
		, grammar(std::move(testGrammar))
		, input(std::move(testInput))
		, expected(testExpected)
	{
	}
};
