#include "Lexer.h"
#include "rules/Identifier.h"
#include "rules/Number.h"
#include "rules/ReservedWords.h"
#include "rules/SpecialChars.h"
#include "rules/String.h"

Lexer::Lexer(std::string const& input)
	: m_reader(input)
{
}

Token Lexer::Get()
{
	if (Empty())
	{
		return Token{
			.type = TokenType::ERROR,
			.pos = m_reader.Count(),
			.line = m_reader.LineCount(),
			.error = Error::EMPTY_INPUT
		};
	}

	const auto ch = m_reader.Peek();

	if (IsIdChar(ch))
	{
		return CheckReserved(Id());
	}
	if (Digit(ch))
	{
		return Number();
	}
	if (IsQuot(ch))
	{
		return String();
	}
	if (IsSpecialChar(ch))
	{
		return SpecialChar();
	}

	return Token{
		.type = TokenType::ERROR,
		.value = std::string{ ch },
		.pos = m_reader.Count(),
		.line = m_reader.LineCount(),
		.error = Error::UNKNOWN_SYMBOL,
	};
}

Token Lexer::Peek()
{
	if (Empty())
	{
		return Token{
			.type = TokenType::ERROR,
			.pos = m_reader.Count(),
			.line = m_reader.LineCount(),
			.error = Error::EMPTY_INPUT
		};
	}
	const auto pos = m_reader.Count();
	const auto token = Get();
	m_reader.Seek(pos);
	return token;
}

bool Lexer::Empty()
{
	SkipWhitespaces();
	return m_reader.EndOfFile();
}

int Lexer::GetLineCount() const
{
	return m_reader.LineCount();
}

Token Lexer::Id()
{
	const auto startPos = m_reader.Count();
	m_reader.Record();
	if (!IdRule(m_reader))
	{
		return Token{
			.type = TokenType::ERROR,
			.pos = startPos,
			.line = m_reader.LineCount(),
			.error = Error::INVALID_ID,
		};
	}

	return Token{
		.type = TokenType::IDENTIFIER,
		.value = m_reader.StopRecord(),
		.pos = startPos,
		.line = m_reader.LineCount()
	};
}

Token Lexer::Number()
{
	const auto startPos = m_reader.Count();
	m_reader.Record();
	bool isInteger = false;
	if (!NumberRule(m_reader, isInteger))
	{
		return Token{
			.type = TokenType::ERROR,
			.pos = startPos,
			.line = m_reader.LineCount(),
			.error = Error::INVALID_NUMBER
		};
	}

	return Token{
		.type = isInteger ? TokenType::INTEGER : TokenType::FLOAT,
		.value = m_reader.StopRecord(),
		.pos = startPos,
		.line = m_reader.LineCount()
	};
}

Token Lexer::String()
{
	const auto startPos = m_reader.Count();
	m_reader.Record();
	if (!StringRule(m_reader))
	{
		return Token{
			.type = TokenType::ERROR,
			.pos = startPos,
			.line = m_reader.LineCount(),
			.error = Error::STRING_LITERAL_INCOMPLETE,
		};
	}

	return Token{
		.type = TokenType::STRING_LITERAL,
		.value = m_reader.StopRecord(),
		.pos = startPos,
		.line = m_reader.LineCount()
	};
}

Token Lexer::SpecialChar()
{
	const auto startPos = m_reader.Count();
	m_reader.Record();
	const auto tokenType = SpecialCharRule(m_reader);
	return Token{
		.type = tokenType,
		.value = m_reader.StopRecord(),
		.pos = startPos,
		.line = m_reader.LineCount(),
	};
}

void Lexer::SkipWhitespaces()
{
	while (!m_reader.EndOfFile() && (m_reader.Peek() == ' ' || m_reader.Peek() == '\n'))
	{
		m_reader.Get();
	}
}
