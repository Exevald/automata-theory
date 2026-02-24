#include "TestRunner.h"

int main()
{
	const auto testCases = TestRunner::CreateTestCases();
	TestRunner::RunAllTests(testCases);

	return 0;
}