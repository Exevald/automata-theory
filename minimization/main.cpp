#include "Machines.h"

#include <iostream>

int main()
{
	try
	{
		const MealyMachine mealy = MealyMachine::FromDotFile("temp.dot");
		std::cout << mealy.ToDotFile() << "\n";

		const MealyMachine minimized = mealy.Minimize();
		minimized.SaveToFile("minimized.dot");
		std::cout << minimized.ToDotFile() << "\n";
	}
	catch (const std::exception& exception)
	{
		std::cerr << "Error: " << exception.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}