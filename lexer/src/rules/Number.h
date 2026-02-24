#pragma once

#include "../Reader.h"

inline bool Digit(const char c)
{
	return c >= '0' && c <= '9';
}

inline bool NonZero(const char c)
{
	return c > '0' && c <= '9';
}

/**
 * expRem -> e | digit expRem
 */
inline bool ExpRem(Reader& reader)
{
	if (reader.Empty())
	{
		return true;
	}

	if (Digit(reader.Peek()))
	{
		reader.Get();
		return ExpRem(reader);
	}

	return true;
}

/**
 * expVal -> nonZero expRem
 */
inline bool ExpVal(Reader& reader)
{
	return !reader.Empty() && NonZero(reader.Get()) && ExpRem(reader);
}

/**
 * exp -> + expVal | - expVal
 */
inline bool Exp(Reader& reader)
{
	if (reader.Empty())
	{
		return false;
	}

	const auto sign = reader.Get();
	return (sign == '+' || sign == '-') && ExpVal(reader);
}

/**
 * expPart -> e | e exp | E exp
 */
inline bool ExpPart(Reader& reader, bool& isInteger)
{
	if (reader.Empty())
	{
		return true;
	}

	const auto ch = reader.Peek();
	if (ch == 'e' || ch == 'E')
	{
		isInteger = false;
		reader.Get();
		return Exp(reader);
	}

	return true;
}

/**
 * mantissaRem -> e | digit mantissaRem
 */
inline bool MantissaRem(Reader& reader)
{
	if (reader.Empty())
	{
		return true;
	}

	if (Digit(reader.Peek()))
	{
		reader.Get();
		return MantissaRem(reader);
	}

	return true;
}

/**
 * mantissa -> digit mantissaRem
 */
inline bool Mantissa(Reader& reader)
{
	if (reader.Empty())
	{
		return false;
	}

	return Digit(reader.Get()) && MantissaRem(reader);
}

/**
 * optMantissa -> e | .mantissa
 */
inline bool OptMantissa(Reader& reader, bool& isInteger)
{
	if (reader.Empty())
	{
		return true;
	}

	if (reader.Peek() == '.')
	{
		isInteger = false;
		reader.Get();
		return Mantissa(reader);
	}

	return !Digit(reader.Peek());
}

/**
 * numberRem -> e | digit numberRem | .mantissa
 */
inline bool NumberRem(Reader& reader, bool& isInteger)
{
	if (reader.Empty())
	{
		return true;
	}

	if (Digit(reader.Peek()))
	{
		reader.Get();
		return NumberRem(reader, isInteger);
	}

	if (reader.Peek() == '.')
	{
		isInteger = false;
		reader.Get();
		return Mantissa(reader);
	}

	return true;
}

/**
 * num -> nonZero numberRem | 0 optMantissa
 */
inline bool Num(Reader& reader, bool& isInteger)
{
	if (reader.Empty())
	{
		return false;
	}

	const auto ch = reader.Peek();
	if (NonZero(ch))
	{
		reader.Get();
		return NumberRem(reader, isInteger);
	}

	if (ch == '0')
	{
		reader.Get();
		return OptMantissa(reader, isInteger);
	}

	return false;
}

/**
 * number -> num expPart
 * num -> nonZero numberRem | 0 optMantissa
 * numberRem -> e | digit numberRem | .mantissa
 * optMantissa -> e | .mantissa
 * mantissa -> digit mantissaRem
 * mantissaRem -> e | digit mantissaRem
 * expPart -> e | e exp | E exp
 * exp -> + expVal | - expVal
 * expVal -> nonZero expRem
 * expRem -> e | digit expRem
 */
inline bool NumberRule(Reader& reader, bool& isInteger)
{
	isInteger = true;
	return Num(reader, isInteger) && ExpPart(reader, isInteger);
}