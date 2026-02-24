#pragma once

#include "Lexer.h"

#include <stdexcept>
#include <string>

class Parser
{
public:
	explicit Parser(Lexer& lexer);
	void Parse();

private:
	void Error(const std::string& msg);
	void Match(TokenType expected, const std::string& context);
	bool TryMatch(TokenType type);
	void ParseBody();
	void ParseDecls();
	void ParseDecl();
	void ParseVarDecl();
	void ParseConstDecl();
	void ParseIdList();
	void ParseStatements();
	void ParseStatement();
	void ParseAssignment();
	void ParseExpr();
	void ParseTerm();
	void ParseFunctor();

	Lexer& m_lexer;
};