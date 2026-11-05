#pragma once

#include "MemoryStreamCompat.h"
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <limits>
#include <span>
#include <string>
#include <type_traits>
#include <vector>

namespace System::IO {

enum class FileMode { OpenRead, Create };

class FileStream final : public Stream {
	std::fstream file_;
	bool readable_ = false;
	bool writable_ = false;
	static std::ios::openmode OpenMode(FileMode mode)
	{
		return mode == FileMode::Create
			? std::ios::binary | std::ios::out | std::ios::trunc
			: std::ios::binary | std::ios::in;
	}
public:
	FileStream(const std::wstring &path, FileMode mode)
		: readable_(mode == FileMode::OpenRead), writable_(mode == FileMode::Create)
	{
		file_.open(std::filesystem::path(path), OpenMode(mode));
		if (!file_) throw std::runtime_error("Unable to open binary trace file.");
	}
	long long getPosition() const override
	{
		auto &file = const_cast<std::fstream&>(file_);
		auto position = writable_ ? file.tellp() : file.tellg();
		if (position == std::streampos(-1)) throw std::runtime_error("Unable to query file position.");
		return static_cast<long long>(position);
	}
	void setPosition(long long value) override { (void)Seek(value, SeekOrigin::Begin); }
	long long getLength() const override
	{
		auto &file = const_cast<std::fstream&>(file_);
		if (writable_)
		{
			auto current = file.tellp(); file.seekp(0, std::ios::end); auto end = file.tellp(); file.seekp(current);
			if (end == std::streampos(-1)) throw std::runtime_error("Unable to query file length.");
			return static_cast<long long>(end);
		}
		auto current = file.tellg(); file.seekg(0, std::ios::end); auto end = file.tellg(); file.seekg(current);
		if (end == std::streampos(-1)) throw std::runtime_error("Unable to query file length.");
		return static_cast<long long>(end);
	}
	bool getCanRead() const override { return readable_; }
	bool getCanWrite() const override { return writable_; }
	bool getCanSeek() const override { return true; }
	void SetLength(long long) override { throw std::runtime_error("FileStream.SetLength is not supported."); }
	void Flush() override { file_.flush(); if (!file_) throw std::runtime_error("File flush failed."); }
	long long Seek(long long offset, SeekOrigin origin) override
	{
		const auto direction = origin == SeekOrigin::Begin ? std::ios::beg : origin == SeekOrigin::Current ? std::ios::cur : std::ios::end;
		file_.clear();
		if (readable_) file_.seekg(offset, direction);
		if (writable_) file_.seekp(offset, direction);
		if (!file_) throw std::runtime_error("File seek failed.");
		return getPosition();
	}
	int Read(std::vector<unsigned char>& buffer, int offset, int count) override
	{
		if (!readable_) throw std::runtime_error("FileStream is not readable.");
		if (offset < 0 || count < 0 || static_cast<size_t>(offset) > buffer.size() || static_cast<size_t>(count) > buffer.size() - static_cast<size_t>(offset)) throw std::out_of_range("Invalid read buffer range.");
		if (count == 0) return 0;
		file_.read(reinterpret_cast<char*>(buffer.data() + offset), count);
		const auto got = file_.gcount();
		if (file_.bad()) throw std::runtime_error("File read failed.");
		if (file_.eof()) file_.clear(file_.rdstate() & ~std::ios::failbit);
		return static_cast<int>(got);
	}
	void Write(std::vector<unsigned char>& buffer, int offset, int count) override
	{
		if (!writable_) throw std::runtime_error("FileStream is not writable.");
		if (offset < 0 || count < 0 || static_cast<size_t>(offset) > buffer.size() || static_cast<size_t>(count) > buffer.size() - static_cast<size_t>(offset)) throw std::out_of_range("Invalid write buffer range.");
		if (count == 0) return;
		file_.write(reinterpret_cast<const char*>(buffer.data() + offset), count);
		if (!file_) throw std::runtime_error("File write failed.");
	}
	int ReadByte() override { std::vector<unsigned char> one(1); return Read(one, 0, 1) == 0 ? -1 : one[0]; }
	void WriteByte(unsigned char value) override { std::vector<unsigned char> one{value}; Write(one, 0, 1); }
};

inline std::string ToUtf8(const std::wstring &text)
{
	std::string out;
	for (size_t i = 0; i < text.size(); ++i)
	{
		uint32_t cp = static_cast<uint32_t>(text[i]);
		if constexpr (sizeof(wchar_t) == 2)
		{
			if (cp >= 0xd800 && cp <= 0xdbff && i + 1 < text.size())
			{
				uint32_t low = static_cast<uint32_t>(text[i + 1]);
				if (low >= 0xdc00 && low <= 0xdfff) { cp = 0x10000 + ((cp - 0xd800) << 10) + low - 0xdc00; ++i; }
			}
		}
		if (cp > 0x10ffff || (cp >= 0xd800 && cp <= 0xdfff)) throw std::runtime_error("Invalid UTF-16 string.");
		if (cp <= 0x7f) out.push_back(static_cast<char>(cp));
		else if (cp <= 0x7ff) { out.push_back(static_cast<char>(0xc0 | (cp >> 6))); out.push_back(static_cast<char>(0x80 | (cp & 0x3f))); }
		else if (cp <= 0xffff) { out.push_back(static_cast<char>(0xe0 | (cp >> 12))); out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3f))); out.push_back(static_cast<char>(0x80 | (cp & 0x3f))); }
		else { out.push_back(static_cast<char>(0xf0 | (cp >> 18))); out.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3f))); out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3f))); out.push_back(static_cast<char>(0x80 | (cp & 0x3f))); }
	}
	return out;
}

inline std::wstring FromUtf8(std::span<const unsigned char> bytes)
{
	std::wstring out;
	for (size_t i = 0; i < bytes.size();)
	{
		const unsigned char lead = bytes[i++]; uint32_t cp; unsigned count;
		if (lead <= 0x7f) { cp = lead; count = 0; }
		else if (lead >= 0xc2 && lead <= 0xdf) { cp = lead & 0x1f; count = 1; }
		else if (lead >= 0xe0 && lead <= 0xef) { cp = lead & 0x0f; count = 2; }
		else if (lead >= 0xf0 && lead <= 0xf4) { cp = lead & 0x07; count = 3; }
		else throw std::runtime_error("Invalid UTF-8 lead byte.");
		if (i + count > bytes.size()) throw std::runtime_error("Truncated UTF-8 sequence.");
		for (unsigned n = 0; n < count; ++n) { const auto b = bytes[i++]; if ((b & 0xc0) != 0x80) throw std::runtime_error("Invalid UTF-8 continuation."); cp = (cp << 6) | (b & 0x3f); }
		if ((count == 1 && cp < 0x80) || (count == 2 && cp < 0x800) || (count == 3 && cp < 0x10000) || cp > 0x10ffff || (cp >= 0xd800 && cp <= 0xdfff)) throw std::runtime_error("Invalid UTF-8 scalar.");
		if constexpr (sizeof(wchar_t) == 2) { if (cp > 0xffff) { cp -= 0x10000; out.push_back(static_cast<wchar_t>(0xd800 + (cp >> 10))); out.push_back(static_cast<wchar_t>(0xdc00 + (cp & 0x3ff))); } else out.push_back(static_cast<wchar_t>(cp)); }
		else out.push_back(static_cast<wchar_t>(cp));
	}
	return out;
}

inline std::wstring FromUtf8Replacing(std::span<const unsigned char> bytes)
{
	std::wstring out;
	auto appendScalar = [&](uint32_t cp)
	{
		if constexpr (sizeof(wchar_t) == 2)
		{
			if (cp > 0xffff) { cp -= 0x10000; out.push_back(static_cast<wchar_t>(0xd800 + (cp >> 10))); out.push_back(static_cast<wchar_t>(0xdc00 + (cp & 0x3ff))); }
			else out.push_back(static_cast<wchar_t>(cp));
		}
		else out.push_back(static_cast<wchar_t>(cp));
	};
	for (size_t i = 0; i < bytes.size();)
	{
		const size_t start = i;
		const unsigned char lead = bytes[i++]; uint32_t cp; unsigned count;
		if (lead <= 0x7f) { appendScalar(lead); continue; }
		if (lead >= 0xc2 && lead <= 0xdf) { cp = lead & 0x1f; count = 1; }
		else if (lead >= 0xe0 && lead <= 0xef) { cp = lead & 0x0f; count = 2; }
		else if (lead >= 0xf0 && lead <= 0xf4) { cp = lead & 0x07; count = 3; }
		else { appendScalar(0xfffd); continue; }
		if (i + count > bytes.size()) { appendScalar(0xfffd); break; }
		bool valid = true;
		for (unsigned n = 0; n < count; ++n)
		{
			const unsigned char byte = bytes[i + n];
			if ((byte & 0xc0) != 0x80) { valid = false; break; }
			cp = (cp << 6) | (byte & 0x3f);
		}
		if (!valid) { i = start + 1; appendScalar(0xfffd); continue; }
		i += count;
		if ((count == 1 && cp < 0x80) || (count == 2 && cp < 0x800) || (count == 3 && cp < 0x10000) || cp > 0x10ffff || (cp >= 0xd800 && cp <= 0xdfff)) appendScalar(0xfffd);
		else appendScalar(cp);
	}
	return out;
}

class BinaryWriter final {
	std::shared_ptr<Stream> stream_;
	void WriteRaw(std::span<const unsigned char> bytes)
	{
		if (bytes.size() > static_cast<size_t>((std::numeric_limits<int>::max)())) throw std::length_error("Binary write too large.");
		std::vector<unsigned char> copy(bytes.begin(), bytes.end()); stream_->Write(copy, 0, static_cast<int>(copy.size()));
	}
public:
	explicit BinaryWriter(std::shared_ptr<Stream> stream) : stream_(std::move(stream)) { if (!stream_ || !stream_->getCanWrite()) throw std::invalid_argument("BinaryWriter requires writable stream."); }
	template<typename T, std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T, bool>, int> = 0>
	void Write(T value)
	{
		using U = std::make_unsigned_t<T>; U bits = static_cast<U>(value); std::array<unsigned char, sizeof(T)> bytes{};
		for (size_t i = 0; i < bytes.size(); ++i) bytes[i] = static_cast<unsigned char>(bits >> (i * 8)); WriteRaw(std::span<const unsigned char>(bytes));
	}
	void Write(bool value) { Write(static_cast<unsigned char>(value ? 1 : 0)); }
	void Write(std::span<const unsigned char> bytes) { WriteRaw(bytes); }
	void Write(const std::vector<unsigned char> &bytes) { WriteRaw(bytes); }
	void Write(const std::wstring &value)
	{
		const std::string utf8 = ToUtf8(value);
		if (utf8.size() > (std::numeric_limits<uint32_t>::max)()) throw std::length_error("String exceeds binary writer length range.");
		uint32_t length = static_cast<uint32_t>(utf8.size());
		while (length >= 0x80) { Write(static_cast<unsigned char>((length & 0x7f) | 0x80)); length >>= 7; }
		Write(static_cast<unsigned char>(length));
		Write(std::span<const unsigned char>(reinterpret_cast<const unsigned char*>(utf8.data()), utf8.size()));
	}
	void Flush() { stream_->Flush(); }
};

class BinaryReader final {
	std::shared_ptr<Stream> stream_;
	bool strictUtf8_ = false;
	void ReadExact(std::span<unsigned char> bytes)
	{
		size_t done = 0;
		while (done < bytes.size())
		{
			const size_t left = bytes.size() - done;
			if (left > static_cast<size_t>((std::numeric_limits<int>::max)())) throw std::length_error("Binary read too large.");
			std::vector<unsigned char> temp(left); const int got = stream_->Read(temp, 0, static_cast<int>(left));
			if (got <= 0) throw std::runtime_error("Unexpected end of binary stream.");
			std::copy_n(temp.begin(), got, bytes.begin() + static_cast<std::ptrdiff_t>(done)); done += static_cast<size_t>(got);
		}
	}
public:
	explicit BinaryReader(std::shared_ptr<Stream> stream, bool strictUtf8 = false) : stream_(std::move(stream)), strictUtf8_(strictUtf8) { if (!stream_ || !stream_->getCanRead()) throw std::invalid_argument("BinaryReader requires readable stream."); }
	template<typename T, std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T, bool>, int> = 0>
	T Read()
	{
		std::array<unsigned char, sizeof(T)> bytes{}; ReadExact(std::span<unsigned char>(bytes)); using U = std::make_unsigned_t<T>; U bits = 0;
		for (size_t i = 0; i < bytes.size(); ++i) bits |= static_cast<U>(bytes[i]) << (i * 8);
		if constexpr (std::is_signed_v<T>) return std::bit_cast<T>(bits);
		else return static_cast<T>(bits);
	}
	unsigned char ReadByte() { return Read<unsigned char>(); }
	unsigned short ReadUInt16() { return Read<unsigned short>(); }
	unsigned int ReadUInt32() { return Read<unsigned int>(); }
	unsigned long long ReadUInt64() { return Read<unsigned long long>(); }
	int ReadInt32() { return Read<int>(); }
	long long ReadInt64() { return Read<long long>(); }
	bool ReadBoolean() { return ReadByte() != 0; }
	std::vector<unsigned char> ReadBytes(int count, size_t maximum = 16u * 1024u * 1024u)
	{
		if (count < 0 || static_cast<size_t>(count) > maximum) throw std::length_error("Binary byte field exceeds limit.");
		std::vector<unsigned char> bytes(static_cast<size_t>(count)); if (count) ReadExact(std::span<unsigned char>(bytes)); return bytes;
	}
	std::vector<unsigned char> ReadBytesExact(int count, size_t maximum) { return ReadBytes(count, maximum); }
	std::wstring ReadString(size_t maximumUtf8Bytes = 1024u * 1024u)
	{
		uint32_t length = 0; unsigned shift = 0;
		for (unsigned i = 0; i < 5; ++i) { const unsigned char b = ReadByte(); if (i == 4 && (b & 0xf0)) throw std::runtime_error("Invalid 7-bit string length."); length |= static_cast<uint32_t>(b & 0x7f) << shift; if (!(b & 0x80)) { if (length > maximumUtf8Bytes || length > static_cast<uint32_t>((std::numeric_limits<int>::max)())) throw std::length_error("String exceeds UTF-8 byte limit."); auto bytes = ReadBytes(static_cast<int>(length), maximumUtf8Bytes); const auto utf8 = std::span<const unsigned char>(bytes); return strictUtf8_ ? FromUtf8(utf8) : FromUtf8Replacing(utf8); } shift += 7; }
		throw std::runtime_error("Invalid 7-bit string length.");
	}
	long long getPosition() const { return stream_->getPosition(); }
	long long getLength() const { return stream_->getLength(); }
	long long getRemaining() const { const auto remaining = getLength() - getPosition(); return remaining < 0 ? 0 : remaining; }
	std::shared_ptr<Stream> getBaseStream() const { return stream_; }
};

} // namespace System::IO
