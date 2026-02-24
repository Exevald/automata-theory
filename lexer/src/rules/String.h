#pragma once

#include "../Reader.h"

inline bool IsQuot(char c)
{
	return c == '\'' || c == '"';
}

inline bool StringRule(Reader& reader)
{
	if (!IsQuot(reader.Get()))
	{
		return false;
	}

	if (reader.Empty())
	{
		return false;
	}

	while (!IsQuot(reader.Get()))
	{
		if (reader.Empty())
		{
			return false;
		}
	}

	return true;
}