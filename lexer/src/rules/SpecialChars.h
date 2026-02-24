#pragma once

#include "../Reader.h"
#include "../Token.h"

#include <unordered_map>

const std::unordered_map<char, TokenType> SPECIAL_CHARS = {
	{ '(', TokenType::PARENTHETHIS_OPEN },
	{ ')', TokenType::PARENTHETHIS_CLOSE },
	{ '{', TokenType::CURLY_OPEN },
	{ '}', TokenType::CURLY_CLOSE },
	{ '[', TokenType::BRACKET_OPEN },
	{ ']', TokenType::BRACKET_CLOSE },
	{ ',', TokenType::COMMA },
	{ '+', TokenType::OP_PLUS },
	{ '-', TokenType::OP_MINUS },
	{ '*', TokenType::OP_MUL },
	{ '/', TokenType::OP_DIVISION },
	{ '=', TokenType::OP_ASSIGNMENT },
	{ '!', TokenType::OP_NOT_MARK },
	{ '<', TokenType::OP_LESS },
	{ '>', TokenType::OP_GREATER },
	{ ';', TokenType::SEMICOLON },
	{ ':', TokenType::COLON },
	{ '.', TokenType::DOT },
};

const std::unordered_map<char, std::pair<char, TokenType>> DOUBLED_SPECIAL_CHARS = {
	{ '=', { '=', TokenType::OP_EQUAL } },
	{ '!', { '=', TokenType::OP_NOT_EQUAL } },
	{ '<', { '=', TokenType::OP_LESS_OR_EQUAL } },
	{ '>', { '=', TokenType::OP_GREATER_OR_EQUAL } },
	{ '-', { '>', TokenType::ARROW } },
	{ '.', { '.', TokenType::DOUBLE_DOT } },
	{ ':', { '=', TokenType::OP_ASSIGN } }
};

inline bool IsSpecialChar(const char c)
{
	return SPECIAL_CHARS.contains(c);
}

inline TokenType SpecialCharRule(Reader& reader)
{
	const auto it = SPECIAL_CHARS.find(reader.Peek());

	reader.Get();
	if (reader.Empty())
	{
		return it->second;
	}

	const auto doubledIt = DOUBLED_SPECIAL_CHARS.find(it->first);
	if (doubledIt == DOUBLED_SPECIAL_CHARS.end() || doubledIt->second.first != reader.Peek())
	{
		return it->second;
	}

	reader.Get();
	return doubledIt->second.second;
}