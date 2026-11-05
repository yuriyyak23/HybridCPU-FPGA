#pragma once

#include <string>
#include <exception>
#include <stdexcept>
#include <codecvt>
#include <locale>
#include <type_traits>
#include <optional>
#include <cwctype>
#include <algorithm>

class ExecutionFaultMetadata {
public:
    virtual ~ExecutionFaultMetadata() = default;
    std::optional<unsigned char> executionFaultCategory;
    std::exception_ptr innerException;
};

class ArgumentNullException : public std::exception
{
private:
	std::string msg;

public:
	explicit ArgumentNullException(const std::string &message = "Value cannot be null.") : msg(message) {}
	explicit ArgumentNullException(const std::wstring &message) : msg(std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(message)) {}
	const char *what() const noexcept override { return msg.c_str(); }
	template<typename T> static void ThrowIfNull(const T &value, const std::string &name = "value")
	{
		// std::string/std::wstring are value types and have no null state. In
		// particular, an empty string is not equivalent to null in .NET.
		if constexpr (std::is_same_v<std::decay_t<T>, std::string> ||
			std::is_same_v<std::decay_t<T>, std::wstring>)
		{
			return;
		}
		else if constexpr (requires { !value; })
		{
			if (!value)
			{
				throw ArgumentNullException(name);
			}
		}
	}
};

class ArgumentOutOfRangeException : public std::exception, public ExecutionFaultMetadata
{
private:
    std::string msg;

public:
    ArgumentOutOfRangeException(const std::wstring &paramName, const std::wstring &message)
    {
        msg = std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(paramName + L": " + message);
    }

    ArgumentOutOfRangeException(const std::string &message = "") : msg(message)
    {
    }

    ArgumentOutOfRangeException(const std::wstring &message)
    {
        msg = std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(message);
    }

    template<typename T> ArgumentOutOfRangeException(const std::wstring &paramName, const T &, const std::wstring &message)
    {
        msg = std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(paramName + L": " + message);
    }

const char * what() const noexcept
    {
        return msg.c_str();
    }
};

class ArgumentException : public std::exception
{
private:
    std::string msg;

public:
    explicit ArgumentException(const std::wstring &message)
        : msg(std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(message)) {}

    const char *what() const noexcept override { return msg.c_str(); }

    static void ThrowIfNullOrWhiteSpace(const std::wstring &value, const std::wstring &paramName = L"value")
    {
        const bool blank = std::all_of(value.begin(), value.end(), [](wchar_t ch) { return std::iswspace(ch) != 0; });
        if (value.empty() || blank)
        {
            throw ArgumentException(paramName + L" must not be null or whitespace.");
        }
    }
};

class DecoderFallbackException : public std::exception
{
private:
    std::string msg;

public:
    DecoderFallbackException(const std::string &message = "") : msg(message)
    {
    }

    DecoderFallbackException(const std::wstring &message)
    {
        msg = std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(message);
    }

const char * what() const noexcept
    {
        return msg.c_str();
    }
};

class EncoderFallbackException : public std::exception
{
private:
    std::string msg;

public:
    EncoderFallbackException(const std::string &message = "") : msg(message)
    {
    }

    EncoderFallbackException(const std::wstring &message)
    {
        msg = std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(message);
    }

const char * what() const noexcept
    {
        return msg.c_str();
    }
};

class EndOfStreamException : public std::exception
{
private:
    std::string msg;

public:
    EndOfStreamException(const std::string &message = "") : msg(message)
    {
    }

    EndOfStreamException(const std::wstring &message)
    {
        msg = std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(message);
    }

const char * what() const noexcept
    {
        return msg.c_str();
    }
};

class FormatException : public std::exception
{
private:
    std::string msg;

public:
    FormatException(const std::string &message = "") : msg(message)
    {
    }

    FormatException(const std::wstring &message)
    {
        msg = std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(message);
    }

const char * what() const noexcept
    {
        return msg.c_str();
    }
};

class InvalidDataException : public std::exception
{
private:
    std::string msg;

public:
    InvalidDataException(const std::string &message = "") : msg(message)
    {
    }

    InvalidDataException(const std::wstring &message)
    {
        msg = std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(message);
    }

const char * what() const noexcept
    {
        return msg.c_str();
    }
};

class InvalidOperationException : public std::runtime_error, public ExecutionFaultMetadata
{
public:
    explicit InvalidOperationException(const std::string& message = "") : std::runtime_error(message) {}
    explicit InvalidOperationException(const std::wstring& message)
        : std::runtime_error(std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(message)) {}
};
class IOException : public std::exception
{
private:
    std::string msg;

public:
    IOException(const std::string &message = "") : msg(message)
    {
    }

    IOException(const std::wstring &message)
    {
        msg = std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(message);
    }

const char * what() const noexcept
    {
        return msg.c_str();
    }
};

class JsonException : public std::exception
{
private:
    std::string msg;

public:
    JsonException(const std::string &message = "") : msg(message)
    {
    }

    JsonException(const std::wstring &message)
    {
        msg = std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(message);
    }

const char * what() const noexcept
    {
        return msg.c_str();
    }
};

class NotSupportedException : public std::exception
{
private:
    std::string msg;

public:
    NotSupportedException(const std::string &message = "") : msg(message)
    {
    }

    NotSupportedException(const std::wstring &message)
    {
        msg = std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(message);
    }

const char * what() const noexcept
    {
        return msg.c_str();
    }
};

class OutOfMemoryException : public std::exception
{
private:
    std::string msg;

public:
    OutOfMemoryException(const std::string &message = "") : msg(message)
    {
    }

    OutOfMemoryException(const std::wstring &message)
    {
        msg = std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(message);
    }

const char * what() const noexcept
    {
        return msg.c_str();
    }
};
