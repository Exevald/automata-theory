#pragma once

#include "../Token.h"

#include <algorithm>
#include <map>
#include <string>

const std::map<std::string, TokenType> RESERVED_WORDS = {
	{ "main", TokenType::MAIN },
	{ "begin", TokenType::BEGIN },
	{ "end", TokenType::END },
	{ "mod", TokenType::OP_MOD },
	{ "div", TokenType::OP_DIV },
	{ "&&", TokenType::OP_AND },
	{ "||", TokenType::OP_OR },
	{ "not", TokenType::OP_NOT },
	{ "true", TokenType::TRUE },
	{ "false", TokenType::FALSE },
	{ "if", TokenType::IF },
	{ "then", TokenType::THEN },
	{ "else", TokenType::ELSE },
	{ "const", TokenType::CONST },
	{ "var", TokenType::VAR },
	{ "function", TokenType::FUNCTION },
	{ "while", TokenType::WHILE },
	{ "return", TokenType::RETURN },
	{ "int", TokenType::TYPE_INTEGER },
	{ "float", TokenType::TYPE_FLOAT },
	{ "string", TokenType::TYPE_STRING },
	{ "bool", TokenType::TYPE_BOOL },
};

inline std::string ToLower(std::string s)
{
	std::ranges::transform(s, s.begin(),
		[](const unsigned char c) { return std::tolower(c); });
	return s;
}

inline Token CheckReserved(Token const& token)
{
	const auto value = ToLower(token.value);
	const auto it = RESERVED_WORDS.find(value);
	return it == RESERVED_WORDS.end()
		? token
		: Token{
			  .type = it->second,
			  .value = value,
			  .pos = token.pos,
			  .error = token.error,
		  };
}