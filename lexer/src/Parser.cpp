#include "Parser.h"

Parser::Parser(Lexer& lexer)
	: m_lexer(lexer)
{
}

void Parser::Parse()
{
	Match(TokenType::MAIN, "Expected 'main'");
	ParseBody();
	Match(TokenType::END, "Expected final 'end' before '.'");
	Match(TokenType::DOT, "Expected '.' at end");
}

void Parser::Error(const std::string& msg)
{
	throw std::runtime_error("Parse error: " + msg + ". Line: " + std::to_string(m_lexer.GetLineCount()));
}

void Parser::Match(TokenType expected, const std::string& context)
{
	if (m_lexer.Peek().type != expected)
	{
		Error(context);
	}
	m_lexer.Get();
}

bool Parser::TryMatch(TokenType type)
{
	if (m_lexer.Peek().type == type)
	{
		m_lexer.Get();
		return true;
	}
	return false;
}

void Parser::ParseBody()
{
	ParseDecls();
	Match(TokenType::BEGIN, "Expected 'begin' in body");
	ParseStatements();
	Match(TokenType::END, "Expected 'end' to close 'begin' block (inner end)");
}

void Parser::ParseDecls()
{
	ParseDecl();
	while (m_lexer.Peek().type == TokenType::SEMICOLON)
	{
		m_lexer.Get();
		if (m_lexer.Peek().type == TokenType::VAR || m_lexer.Peek().type == TokenType::CONST)
		{
			ParseDecl();
		}
		else
		{
			break;
		}
	}
}

void Parser::ParseDecl()
{
	if (m_lexer.Peek().type == TokenType::VAR)
	{
		m_lexer.Get();
		ParseVarDecl();
	}
	else if (m_lexer.Peek().type == TokenType::CONST)
	{
		m_lexer.Get();
		ParseConstDecl();
	}
	else
	{
		Error("Expected 'var' or 'const'");
	}
}

void Parser::ParseVarDecl()
{
	ParseIdList();
	Match(TokenType::COLON, "Expected ':' after identifier list");
	if (!TryMatch(TokenType::TYPE_INTEGER) && !TryMatch(TokenType::TYPE_FLOAT))
	{
		Error("Expected type 'int' or 'float'");
	}
}

void Parser::ParseConstDecl()
{
	Match(TokenType::IDENTIFIER, "Expected identifier in const declaration");
	Match(TokenType::OP_ASSIGNMENT, "Expected '=' in const (e.g., const x = 5)");
	ParseExpr();
}

void Parser::ParseIdList()
{
	Match(TokenType::IDENTIFIER, "Expected identifier");
	while (TryMatch(TokenType::COMMA))
	{
		Match(TokenType::IDENTIFIER, "Expected identifier after comma");
	}
}

void Parser::ParseStatements()
{
	ParseStatement();
	while (TryMatch(TokenType::SEMICOLON))
	{
		if (m_lexer.Peek().type == TokenType::IDENTIFIER)
		{
			ParseStatement();
		}
		else
		{
			break;
		}
	}
}

void Parser::ParseStatement()
{
	ParseAssignment();
}

void Parser::ParseAssignment()
{
	Match(TokenType::IDENTIFIER, "Expected lvalue (identifier) before ':='");
	Match(TokenType::OP_ASSIGN, "Expected ':=' for assignment");
	ParseExpr();
}

void Parser::ParseExpr()
{
	ParseTerm();
	while (TryMatch(TokenType::OP_PLUS) || TryMatch(TokenType::OP_MINUS))
	{
		ParseTerm();
	}
}

void Parser::ParseTerm()
{
	ParseFunctor();
	while (TryMatch(TokenType::OP_MUL) || TryMatch(TokenType::OP_DIVISION))
	{
		ParseFunctor();
	}
}

void Parser::ParseFunctor()
{
	if (TryMatch(TokenType::OP_MINUS))
	{
		ParseFunctor();
	}
	else if (TryMatch(TokenType::PARENTHETHIS_OPEN))
	{
		ParseExpr();
		Match(TokenType::PARENTHETHIS_CLOSE, "Expected ')' after expression");
	}
	else if (m_lexer.Peek().type == TokenType::IDENTIFIER
		|| m_lexer.Peek().type == TokenType::INTEGER
		|| m_lexer.Peek().type == TokenType::FLOAT)
	{
		m_lexer.Get();
	}
	else
	{
		Error("Expected identifier, number, or (expression)");
	}
}
