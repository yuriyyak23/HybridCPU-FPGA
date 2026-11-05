#pragma once
#include <algorithm>
#include <limits>
#include <memory>
#include <span>
#include <stdexcept>
#include <vector>

namespace System::IO {
enum class SeekOrigin { Begin, Current, End };
class Stream : public std::enable_shared_from_this<Stream> {
public:
    virtual ~Stream() = default;
    virtual long long getPosition() const = 0;
    virtual void setPosition(long long value) = 0;
    virtual long long getLength() const = 0;
    virtual bool getCanRead() const = 0;
    virtual bool getCanWrite() const = 0;
    virtual bool getCanSeek() const = 0;
    virtual void SetLength(long long value) = 0;
    virtual void Flush() = 0;
    virtual long long Seek(long long offset, SeekOrigin origin) = 0;
    virtual int Read(std::vector<unsigned char>& buffer, int offset, int count) = 0;
    virtual void Write(std::vector<unsigned char>& buffer, int offset, int count) = 0;
    virtual int ReadByte() = 0;
    virtual void WriteByte(unsigned char value) = 0;
};
class MemoryStream : public Stream {
    std::vector<unsigned char> data_;
    long long position_ = 0;
    bool expandable_ = true;
    static void checkRange(size_t length, int offset, int count) {
        if (offset < 0 || count < 0 || static_cast<size_t>(offset) > length || static_cast<size_t>(count) > length - static_cast<size_t>(offset))
            throw std::out_of_range("Invalid buffer range");
    }
public:
    MemoryStream() = default;
    explicit MemoryStream(std::vector<unsigned char> data) : data_(std::move(data)), expandable_(false) {}
    long long getPosition() const override { return position_; }
    void setPosition(long long value) override { if (value < 0) throw std::out_of_range("Negative position"); position_ = value; }
    long long getLength() const override { return static_cast<long long>(data_.size()); }
    int getCapacity() const { return static_cast<int>(data_.capacity()); }
    void setCapacity(int value) { if (value < 0 || static_cast<size_t>(value) < data_.size()) throw std::out_of_range("Invalid capacity"); if (!expandable_ && static_cast<size_t>(value) != data_.capacity()) throw std::runtime_error("Fixed memory stream"); data_.reserve(static_cast<size_t>(value)); }
    bool getCanRead() const override { return true; }
    bool getCanWrite() const override { return true; }
    bool getCanSeek() const override { return true; }
    void SetLength(long long value) override { if (value < 0 || (!expandable_ && static_cast<unsigned long long>(value) > data_.capacity())) throw std::out_of_range("Invalid stream length"); data_.resize(static_cast<size_t>(value)); if (position_ > value) position_ = value; }
    void Flush() override {}
    long long Seek(long long offset, SeekOrigin origin) override {
        const long long base = origin == SeekOrigin::Begin ? 0 : origin == SeekOrigin::Current ? position_ : getLength();
        if ((offset > 0 && base > (std::numeric_limits<long long>::max)() - offset) || (offset < 0 && offset < -base)) throw std::out_of_range("Invalid seek");
        setPosition(base + offset); return position_;
    }
    int Read(std::vector<unsigned char>& buffer, int offset, int count) override { checkRange(buffer.size(), offset, count); if (position_ >= getLength()) return 0; const auto n = (std::min)(static_cast<long long>(count), getLength() - position_); std::copy_n(data_.begin() + position_, static_cast<size_t>(n), buffer.begin() + offset); position_ += n; return static_cast<int>(n); }
    int Read(std::span<unsigned char> buffer) { if (position_ >= getLength()) return 0; const auto n = (std::min)(buffer.size(), static_cast<size_t>(getLength() - position_)); std::copy_n(data_.begin() + position_, n, buffer.begin()); position_ += static_cast<long long>(n); return static_cast<int>(n); }
    void Write(std::vector<unsigned char>& buffer, int offset, int count) override { checkRange(buffer.size(), offset, count); if (position_ > (std::numeric_limits<long long>::max)() - count) throw std::out_of_range("Stream too large"); auto end = position_ + count; if (end > getLength()) { auto old = position_; SetLength(end); position_ = old; } std::copy_n(buffer.begin() + offset, count, data_.begin() + position_); position_ = end; }
    int ReadByte() override { if (position_ >= getLength()) return -1; return data_[static_cast<size_t>(position_++)]; }
    void WriteByte(unsigned char value) override { std::vector<unsigned char> one{value}; Write(one, 0, 1); }
    std::vector<unsigned char> ToArray() const { return data_; }
};
}