#pragma once

// ----------------------------------------------------------------------------------------
// Copyright © 2004 - 2025 Tangible Software Solutions Inc.
// This class can be used by anyone provided that the copyright notice remains intact.
//
// This class is used to replace some string methods, including
// conversions to or from strings.
// ----------------------------------------------------------------------------------------
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

class StringHelper
{
public:
	static std::wstring toLower(std::wstring source)
	{
		std::transform(source.begin(), source.end(), source.begin(), [](unsigned char c){ return std::tolower(c); });
		return source;
	}

	static std::wstring toUpper(std::wstring source)
	{
		std::transform(source.begin(), source.end(), source.begin(), [](unsigned char c){ return std::toupper(c); });
		return source;
	}

	static std::wstring trimStart(std::wstring source, const std::wstring &trimChars = L" \t\n\r\v\f")
	{
		std::size_t first = 0;
        while (first < source.size()) {
            bool matched = false;
            for (wchar_t ch : trimChars) if (source[first] == ch) { matched = true; break; }
            if (!matched) break;
            ++first;
        }
        return source.erase(0, first);
	}

	static std::wstring trimEnd(std::wstring source, const std::wstring &trimChars = L" \t\n\r\v\f")
	{
		std::size_t last = source.size();
        while (last > 0) {
            bool matched = false;
            for (wchar_t ch : trimChars) if (source[last - 1] == ch) { matched = true; break; }
            if (!matched) break;
            --last;
        }
        return source.erase(last);
	}

	static std::wstring trim(std::wstring source, const std::wstring &trimChars = L" \t\n\r\v\f")
	{
		return trimStart(trimEnd(source, trimChars), trimChars);
	}

	static std::wstring replace(std::wstring source, const std::wstring &find, const std::wstring &replace)
	{
		std::size_t pos = 0;
		while ((pos = source.find(find, pos)) != std::wstring::npos)
		{
			source.replace(pos, find.length(), replace);
			pos += replace.length();
		}
		return source;
	}

	static bool startsWith(const std::wstring &source, const std::wstring &value)
	{
		if (source.length() < value.length())
			return false;
		else
			return source.compare(0, value.length(), value) == 0;
	}

	static bool endsWith(const std::wstring &source, const std::wstring &value)
	{
		if (source.length() < value.length())
			return false;
		else
			return source.compare(source.length() - value.length(), value.length(), value) == 0;
	}

	static std::vector<std::wstring> split(const std::wstring &source, wchar_t delimiter)
	{
		std::vector<std::wstring> output;
		std::wistringstream ss(source);
		std::wstring nextItem;

		while (std::getline(ss, nextItem, delimiter))
		{
			output.push_back(nextItem);
		}

		return output;
	}

	template<typename T>
	static std::wstring toString(const T &subject)
	{
		std::wostringstream ss;
		ss << subject;
		return ss.str();
	}

	template<typename T>
	static T fromString(const std::wstring &subject)
	{
		std::wistringstream ss(subject);
		T target;
		ss >> target;
		return target;
	}

	static bool isEmptyOrWhiteSpace(const std::wstring &source)
	{
		if (source.length() == 0)
			return true;
		else
		{
			for (std::size_t index = 0; index < source.length(); index++)
			{
				if (!std::isspace(source[index]))
					return false;
			}

			return true;
		}
	}
};
