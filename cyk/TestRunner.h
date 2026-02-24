#pragma once

#include "TestCase.h"

#include <vector>

namespace TestRunner
{
std::vector<TestCase> CreateTestCases();
void RunAllTests(const std::vector<TestCase>& testCases);
bool RunTest(const TestCase& testCase);
}; // namespace TestRunner
