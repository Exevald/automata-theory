#include "Lexer.h"
#include "Parser.h"

int main()
{
	Lexer lexer("main\n"
				"  var a, b, c: int;\n"
				"  var x, y: float;\n"
				"  const z = 52;\n"
				"  begin\n"
				"    x := x + (-a * -c) + y;\n"
				"	 y := z+(-(-6))    * (((-(((-3123123456765432345)))+54343.32321324)));"
				"  end\n"
				"end.");

	auto parser = Parser(lexer);
	parser.Parse();
}