#pragma once

// ----------------------------------------------------------------------------------------
// Copyright © 2004 - 2025 Tangible Software Solutions Inc.
// This class can be used by anyone provided that the copyright notice remains intact.
//
// This class is used to replace the .NET StringBuilder in C++.
// ----------------------------------------------------------------------------------------
#include <string>
#include <sstream>
#include <memory>
#include <codecvt>
#include <locale>

class StringBuilder : public std::enable_shared_from_this<StringBuilder>
{
private:
	std::wstring privateString;

public:
	StringBuilder()
	{
	}

	StringBuilder(const std::wstring &initialString)
	{
		privateString = initialString;
	}

	StringBuilder(std::size_t capacity)
	{
		ensureCapacity(capacity);
	}

	StringBuilder(const std::wstring &initialString, std::size_t capacity)
	{
		privateString = initialString;
		ensureCapacity(capacity);
	}

	wchar_t operator [](std::size_t index)
	{
		return privateString[index];
	}

	std::shared_ptr<StringBuilder> append(const std::wstring &toAppend)
	{
		privateString += toAppend;
		return shared_from_this();
	}

	template<typename T>
	std::shared_ptr<StringBuilder> append(const T &toAppend)
	{
		privateString += toString(toAppend);
		return shared_from_this();
	}

	std::shared_ptr<StringBuilder> appendLine()
	{
		privateString += L"\r\n";
		return shared_from_this();
	}

	std::shared_ptr<StringBuilder> appendLine(const std::wstring &toAppend)
	{
		privateString += toAppend + L"\r\n";
		return shared_from_this();
	}

	std::shared_ptr<StringBuilder> appendLine(const std::string &toAppend)
	{
		privateString += std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().from_bytes(toAppend) + L"\r\n";
		return shared_from_this();
	}

	std::shared_ptr<StringBuilder> insert(std::size_t position, const std::wstring &toInsert)
	{
		privateString.insert(position, toInsert);
		return shared_from_this();
	}

	template<typename T>
	std::shared_ptr<StringBuilder> insert(std::size_t position, const T &toInsert)
	{
		privateString.insert(position, toString(toInsert));
		return shared_from_this();
	}

	std::wstring toString()
	{
		return privateString;
	}

	std::wstring toString(std::size_t start, std::size_t length)
	{
		return privateString.substr(start, length);
	}

	std::size_t length()
	{
		return privateString.length();
	}

	void setLength(std::size_t newLength)
	{
		privateString.resize(newLength);
	}

	std::size_t capacity()
	{
		return privateString.capacity();
	}

	void ensureCapacity(std::size_t minimumCapacity)
	{
		privateString.reserve(minimumCapacity);
	}

	std::size_t maxCapacity()
	{
		return privateString.max_size();
	}

	void clear()
	{
		privateString.clear();
	}

	std::shared_ptr<StringBuilder> remove(std::size_t start, std::size_t length)
	{
		privateString.erase(start, length);
		return shared_from_this();
	}

	std::shared_ptr<StringBuilder> replace(const std::wstring &oldString, const std::wstring &newString)
	{
		std::size_t pos = 0;
		while ((pos = privateString.find(oldString, pos)) != std::wstring::npos)
		{
			privateString.replace(pos, oldString.length(), newString);
			pos += newString.length();
		}
		return shared_from_this();
	}

private:
	template<typename T>
	static std::wstring toString(const T &subject)
	{
		std::wostringstream ss;
		ss << subject;
		return ss.str();
	}
};
