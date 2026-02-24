#include "Reader.h"

Reader::Reader(const std::string& str)
	: m_input(str)
{
}

Reader::Reader(const std::istream& strm)
{
	m_input << strm.rdbuf();
}

char Reader::Get()
{
	if (EndOfFile())
	{
		throw std::runtime_error("EOF Error: tried to get char from an empty reader");
	}
	const auto ch = static_cast<char>(m_input.get());
	if (ch == '\n')
	{
		++m_lineCount;
		m_count = 0;
	}
	else
	{
		++m_count;
	}
	m_record += ch;

	return ch;
}

char Reader::Peek()
{
	return static_cast<char>(m_input.peek());
}

void Reader::Unget()
{
	--m_count;
	m_record.pop_back();
	m_input.unget();
}

size_t Reader::Count() const
{
	return m_count;
}

size_t Reader::LineCount() const
{
	return m_lineCount;
}

bool Reader::Empty()
{
	return m_input.peek() == '\n' || m_input.eof();
}

bool Reader::EndOfFile()
{
	m_input.peek();
	return m_input.eof();
}

void Reader::Seek(const size_t pos)
{
	const size_t dropLen = m_count - pos;
	if (dropLen <= m_record.size())
	{
		m_record.resize(m_record.size() - dropLen);
	}
	else
	{
		m_record.clear();
	}
	const auto offset = pos - m_count;
	m_count = pos;
	m_input.clear();
	m_input.seekg(static_cast<long>(offset), std::ios_base::cur);
}

void Reader::Record()
{
	m_record.clear();
}

std::string Reader::StopRecord()
{
	return m_record;
}
