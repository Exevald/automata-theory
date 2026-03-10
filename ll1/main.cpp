#include "GrammarProcessor.h"
#include "TableGenerator.h"

#include <iostream>

int main()
{
	try
	{
		const auto prods = GrammarProcessor::ParseFile("grammar.txt");
		TableGenerator gen(prods);
		gen.Generate();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	return 0;
}