#pragma once

#include "Token.h"
#include "Reader.h"

class Lexer
{
public:
	explicit Lexer(std::string const& input);
	Token Get();
	Token Peek();
	[[nodiscard]] bool Empty();
	int GetLineCount() const;

private:
	Token Id();
	Token Number();
	Token String();
	Token SpecialChar();
	void SkipWhitespaces();

private:
	Reader m_reader;
};