#pragma once

#include "Error.h"
#include <string>

enum class TokenType
{
	ERROR,
	MAIN, // main
	BEGIN, // begin
	END, //end
	PARENTHETHIS_OPEN, // (
	PARENTHETHIS_CLOSE, // )
	CURLY_OPEN, // {
	CURLY_CLOSE, // }
	BRACKET_OPEN, // [
	BRACKET_CLOSE, // ]
	COMMA, // ,
	OP_PLUS, // +
	OP_MINUS, // -
	OP_MUL, // *
	OP_MOD, // mod
	OP_DIV, // div
	OP_DIVISION, // /
	OP_ASSIGNMENT, // =
	OP_EQUAL, // ==
	OP_NOT_EQUAL, // !=
	OP_LESS, // <
	OP_GREATER, // >
	OP_LESS_OR_EQUAL, // <=
	OP_GREATER_OR_EQUAL, // >=
	OP_AND, // and
	OP_OR, // or
	OP_NOT, // not
	OP_NOT_MARK, // !
	IDENTIFIER, // identifier
	INTEGER, // integer
	FLOAT, // float
	STRING_LITERAL, // string literal
	TRUE, // true
	FALSE, // false
	IF, // if
	SEMICOLON, // ;
	COLON, // :
	THEN, // then
	ELSE, // else
	CONST, // const
	VAR, // var
	FUNCTION, // function
	WHILE, // while
	RETURN, // return
	ARROW, // ->
	OP_ASSIGN, // :=
	TYPE_INTEGER,
	TYPE_FLOAT,
	TYPE_BOOL,
	TYPE_STRING,
	DOUBLE_DOT,
	DOT,
};

struct Token
{
	TokenType type;
	std::string value;
	size_t pos;
	size_t line;
	Error error = Error::NONE;
};

inline bool operator==(Token const& left, Token const& right)
{
	return left.type == right.type
		&& left.value == right.value
		&& left.pos == right.pos
		&& left.error == right.error;
}