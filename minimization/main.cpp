#include "Machines.h"

#include <iostream>

int main()
{
	try
	{
		MealyMachine mealy = MealyMachine::FromDotFile("temp.dot");
		std::cout << mealy.ToDotFile() << "\n";

		auto minized = mealy.Minimize();
		minized.SaveToFile("minimized.dot");
	}
	catch (const std::exception& exception)
	{
		std::cerr << "Error: " << exception.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}