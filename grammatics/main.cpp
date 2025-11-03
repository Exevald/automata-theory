#include "Grammar.h"
#include "Machines.h"
#include <iostream>
#include <string>

int main()
{
	try
	{
		Grammar grammar = Grammar::FromFile("grammar.gram");
		const MealyMachine nfa = grammar.ToNFA();
		nfa.SaveToFile("nfa.dot");
		const auto dfa = nfa.Determinize();
		dfa.SaveToFile("dfa.dot");
		// auto moore = MealyMachine::FromDotFile("input.dot");
		// moore.SaveToFile("mealy.dot");
		// auto min = moore.Minimize();
		// min.SaveToFile("min.dot");
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << "\n";
	}
	return 0;
}