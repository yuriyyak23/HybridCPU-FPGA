#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#include <bcrypt.h>
#include <array>
#include <compare>
#include <stdexcept>
#include <string>

// Value identity generated using the system cryptographic random provider.
class Guid final {
    std::array<unsigned char, 16> bytes_{};
public:
    Guid() = default;
    // Mirrors the .NET Guid(ReadOnlySpan<byte>) constructor used by
    // deterministic external-operation correlation IDs.
    explicit Guid(const std::array<unsigned char, 16>& bytes) : bytes_(bytes) {}
    static const Guid Empty;
    static Guid NewGuid() {
        Guid value;
        if (BCryptGenRandom(nullptr, value.bytes_.data(), static_cast<ULONG>(value.bytes_.size()), BCRYPT_USE_SYSTEM_PREFERRED_RNG) < 0)
            throw std::runtime_error("Unable to generate Guid");
        value.bytes_[6] = static_cast<unsigned char>((value.bytes_[6] & 0x0f) | 0x40);
        value.bytes_[8] = static_cast<unsigned char>((value.bytes_[8] & 0x3f) | 0x80);
        return value;
    }
    std::wstring ToString(const std::wstring& format = L"D") const {
        const bool compact = format == L"N" || format == L"n";
        if (!compact && format != L"D" && format != L"d") throw std::invalid_argument("Unsupported Guid format");
        static constexpr wchar_t hex[] = L"0123456789abcdef";
        std::wstring result; result.reserve(compact ? 32 : 36);
        for (std::size_t i = 0; i < bytes_.size(); ++i) {
            if (!compact && (i == 4 || i == 6 || i == 8 || i == 10)) result.push_back(L'-');
            result.push_back(hex[bytes_[i] >> 4]); result.push_back(hex[bytes_[i] & 15]);
        }
        return result;
    }
    bool operator==(const Guid&) const = default;
    auto operator<=>(const Guid&) const = default;
};
inline const Guid Guid::Empty{};
