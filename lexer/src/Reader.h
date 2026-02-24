#pragma once

#include <iostream>
#include <sstream>

class Reader
{
public:
	explicit Reader(const std::string& str);
	explicit Reader(const std::istream& strm);

	void Unget();
	char Get();

	[[nodiscard]] char Peek();
	void Seek(size_t pos);

	[[nodiscard]] size_t Count() const;
	[[nodiscard]] size_t LineCount() const;

	[[nodiscard]] bool Empty();
	[[nodiscard]] bool EndOfFile();

	void Record();
	std::string StopRecord();

private:
	std::stringstream m_input;
	size_t m_count = 0;
	size_t m_lineCount = 0;
	std::string m_record;
};