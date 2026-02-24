#include "TestRunner.h"
#include "CNFConverter.h"
#include "CYKParser.h"

#include <iomanip>
#include <iostream>

namespace
{
void PrintTestResult(const TestCase& testCase, bool passed)
{
	constexpr int BOX_WIDTH = 60;

	std::cout << "\n┌" << std::string(BOX_WIDTH - 2, '=') << "┐" << std::endl;

	if (passed)
	{
		std::cout << "║  ✓ TEST #" << std::setw(2) << testCase.id
				  << " PASSED"
				  << std::setw(BOX_WIDTH - 20) << ""
				  << " ║" << std::endl;
	}
	else
	{
		std::cout << "║  ✗ TEST #" << std::setw(2) << testCase.id
				  << " FAILED (expected: "
				  << (testCase.expected ? "true" : "false") << ")"
				  << std::setw(BOX_WIDTH - 35) << ""
				  << " ║" << std::endl;
	}

	std::cout << "└" << std::string(BOX_WIDTH - 2, '=') << "┘" << std::endl;
}
} // namespace

std::vector<TestCase> TestRunner::CreateTestCases()
{
	std::vector<TestCase> tests;

	{
		Grammar g;
		g["S"] = { { "A", "S" }, { "B", "S" }, { "a" } };
		g["A"] = { { "B", "B" }, { "a" } };
		g["B"] = { { "A", "A" }, { "b" } };

		tests.emplace_back(
			1, "TEST 1: w = aabaa",
			"S→AS|BS|a, A→BB|a, B→AA|b",
			g, "aabaa", true);
	}
	{
		Grammar g;
		g["S"] = { { "A", "A" }, { "0" } };
		g["A"] = { { "S", "S" }, { "1" } };

		tests.emplace_back(
			2, "TEST 2: w = 110001",
			"S→AA|0, A→SS|1",
			g, "110001", false);
	}
	{
		Grammar g;
		g["S"] = { { "S", "S" }, { "L", "S", "R" }, { "L", "R" } };
		g["L"] = { { "(" } };
		g["R"] = { { ")" } };

		tests.emplace_back(
			3, "TEST 3: w = ()(())",
			"S→SS|(S)|()",
			g, "()(())", true);
	}
	{
		Grammar g;
		g["S"] = { { "S", "S" }, { "L", "S", "R" }, { "L", "R" } };
		g["L"] = { { "(" } };
		g["R"] = { { ")" } };

		tests.emplace_back(
			4, "TEST 4: w = ())( ",
			"S→SS|(S)|()",
			g, "())(", false);
	}
	{
		Grammar g;
		g["S"] = { { "A", "B" }, { "B", "C" } };
		g["A"] = { { "B", "A" }, { "a" } };
		g["B"] = { { "C", "C" }, { "b" } };
		g["C"] = { { "A", "B" }, { "a" } };

		tests.emplace_back(
			5, "TEST 5: w = baaba",
			"S→AB|BC, A→BA|a, B→CC|b, C→AB|a",
			g, "baaba", true);
	}
	{
		Grammar g;
		g["S"] = { { "A", "S" }, { "B", "S" }, { "a" } };
		g["A"] = { { "B", "B" }, { "a" } };
		g["B"] = { { "A", "A" }, { "b" } };

		tests.emplace_back(
			6, "TEST 6: w = aaaaa",
			"S→AS|BS|a, A→BB|a, B→AA|b",
			g, "aaaaa", true);
	}
	{
		Grammar g;
		g["S"] = { { "A", "S" }, { "b" } };
		g["A"] = { { "S", "A" }, { "a" } };

		tests.emplace_back(
			7, "TEST 7: w = babab",
			"S→AS|b, A→SA|a",
			g, "babab", true);
	}
	{
		Grammar g;
		g["S"] = { { "A", "B" }, { "B", "A" }, { "S", "S" }, { "b" } };
		g["A"] = { { "a" } };
		g["B"] = { { "b" } };

		tests.emplace_back(
			8, "TEST 8: w = abab",
			"S→AB|BA|SS|b, A→a, B→b",
			g, "abab", true);
	}
	{
		Grammar g;
		g["S"] = { { "A", "B" } };
		g["A"] = { { "a" }, { EPSILON } };
		g["B"] = { { "b" }, { EPSILON } };

		tests.emplace_back(
			9, "TEST 9: ε-rules",
			"S→AB, A→a|ε, B→b|ε",
			g, "ab", true);
	}
	{
		Grammar g;
		g["S"] = { { "A", "S" }, { "B", "S" }, { "a" } };
		g["A"] = { { "B", "B" }, { "a" } };
		g["B"] = { { "A", "A" }, { "b" } };

		tests.emplace_back(
			10, "TEST 10: w = abbaaa",
			"Example from presentation",
			g, "abbaaa", true);
	}

	return tests;
}

bool TestRunner::RunTest(const TestCase& testCase)
{
	std::cout << "\n┌" << std::string(60, '-') << "┐" << std::endl;
	std::cout << "│ " << std::left << std::setw(58) << testCase.name << " │" << std::endl;
	std::cout << "└" << std::string(60, '-') << "┘" << std::endl;

	std::cout << "Description: " << testCase.description << std::endl;
	std::cout << "Input: \"" << testCase.input << "\"" << std::endl;
	std::cout << "Expected: " << (testCase.expected ? "BELONGS" : "NOT BELONGS") << std::endl;

	const Grammar cnfGrammar = CNFConverter::ConvertToCNF(testCase.grammar);
	const bool result = CYKParser::Parse(cnfGrammar, testCase.input);

	const bool passed = (result == testCase.expected);
	PrintTestResult(testCase, passed);

	return passed;
}

void TestRunner::RunAllTests(const std::vector<TestCase>& testCases)
{
	for (const auto& testCase : testCases)
	{
		RunTest(testCase);
	}
}
