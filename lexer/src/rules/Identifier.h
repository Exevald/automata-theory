#pragma once

#include "../Reader.h"

inline bool IsIdChar(char ch)
{
	return ch == '_' || ch == '$' || std::isalpha(ch);
}

/**
 * idChar -> _ | $ | letter
 */
inline bool IdChar(Reader& reader)
{
	return IsIdChar(reader.Get());
}

/**
 * idPart -> idChar | digit
 */
inline bool IdPart(Reader& reader)
{
	if (IdChar(reader))
	{
		return true;
	}
	reader.Unget();

	return std::isdigit(reader.Get());
}

/**
 * simpleIdRemainder -> e | idPart simpleIdRemainder
 */
inline bool SimpleIdRemainder(Reader& reader)
{
	if (reader.Empty())
	{
		return true;
	}

	const auto count = reader.Count();
	if (!IdPart(reader))
	{
		reader.Seek(count);
		return true;
	}

	return SimpleIdRemainder(reader);
}

inline bool SimpleId(Reader& reader)
{
	if (reader.Empty()) return false;
	if (!IsIdChar(reader.Get())) return false;

	while (!reader.Empty())
	{
		char c = reader.Peek();
		if (IsIdChar(c) || std::isdigit(static_cast<unsigned char>(c)))
		{
			reader.Get();
		}
		else
		{
			break;
		}
	}
	return true;
}

inline bool IdRule(Reader& reader)
{
	return SimpleId(reader);
}