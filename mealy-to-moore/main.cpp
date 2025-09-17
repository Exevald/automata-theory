#include "Machines.h"

#include <iostream>

int main()
{
	try
	{
		MealyMachine mealy = MealyMachine::FromDotFile("input.dot");
		std::cout << mealy.ToDotFile() << "\n";

		MooreMachine moore(mealy);
		std::cout << moore.ToDot() << "\n";
		moore.SaveToFile("moore_output.dot");

		MealyMachine mealy_back(moore);
		std::cout << mealy_back.ToDotFile() << "\n";
		mealy_back.SaveToFile("mealy_back.dot");
	}
	catch (const std::exception& exception)
	{
		std::cerr << "Error: " << exception.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}