#include "Machines.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

std::string sanitize_filename(const std::string& input)
{
	std::string sanitized = input;
	for (char& c : sanitized)
	{
		if (c == '*' || c == '+' || c == '(' || c == ')' || c == '|' || c == '[' || c == ']' || c == '?' || c == '^' || c == '$' || c == '.')
		{
			c = '_';
		}
	}
	std::replace(sanitized.begin(), sanitized.end(), ' ', '_');
	return sanitized;
}

int main()
{
	const std::vector<std::string> regexes = {
		""
	};

	for (const auto& regex : regexes)
	{
		try
		{
			std::cout << "1. Parsing regex into NKA (MooreMachine)..." << std::endl;
			MooreMachine nka = MooreMachine::FromRegex(regex);
			std::cout << "    NKA created. States: " << nka.GetStates().size() << std::endl;

			std::cout << "2. Converting NKA (MooreMachine) to DFA (MealyMachine)..." << std::endl;
			MealyMachine dka = nka.ToDFA();
			std::cout << "    DFA created. States: " << dka.GetStates().size() << std::endl;
			dka.SaveToFile("dka_" + sanitize_filename(regex) + ".dot");

			std::cout << "3. Minimizing DFA (MealyMachine)..." << std::endl;
			MealyMachine minimizedDka = dka.Minimize();

			std::cout << "    Minimized DFA created. States: " << minimizedDka.GetStates().size() << std::endl;
			minimizedDka.SaveToFile("min_dka_" + sanitize_filename(regex) + ".dot");

			std::cout << "Success for regex: " << regex << std::endl
					  << std::endl;
		}
		catch (const std::exception& e)
		{
			std::cerr << "Error processing regex '" << regex << "': " << e.what() << std::endl
					  << std::endl;
		}
	}

	return 0;
}