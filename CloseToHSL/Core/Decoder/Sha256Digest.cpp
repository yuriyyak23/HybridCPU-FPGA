#include "Sha256Digest.h"
#include <windows.h>
#include <bcrypt.h>
#include <algorithm>
#include <array>
#include <limits>
#include <stdexcept>
#include <vector>

namespace YAKSys_Hybrid_CPU::Core::Decoder {
namespace {
void Check(NTSTATUS status) { if (status < 0) throw std::runtime_error("BCrypt SHA-256 failed"); }
struct Algorithm { BCRYPT_ALG_HANDLE h = nullptr; ~Algorithm() { if (h) BCryptCloseAlgorithmProvider(h, 0); } };
struct Hash { BCRYPT_HASH_HANDLE h = nullptr; ~Hash() { if (h) BCryptDestroyHash(h); } };
}
std::wstring Sha256Hex(std::span<const unsigned char> bytes) {
    Algorithm algorithm;
    Check(BCryptOpenAlgorithmProvider(&algorithm.h, BCRYPT_SHA256_ALGORITHM, nullptr, 0));
    DWORD size = 0, returned = 0;
    Check(BCryptGetProperty(algorithm.h, BCRYPT_OBJECT_LENGTH, reinterpret_cast<PUCHAR>(&size), sizeof(size), &returned, 0));
    std::vector<unsigned char> storage(size);
    Hash hash;
    Check(BCryptCreateHash(algorithm.h, &hash.h, storage.data(), size, nullptr, 0, 0));
    while (!bytes.empty()) {
        auto count = static_cast<ULONG>((std::min)(bytes.size(), static_cast<std::size_t>((std::numeric_limits<ULONG>::max)())));
        Check(BCryptHashData(hash.h, const_cast<PUCHAR>(bytes.data()), count, 0));
        bytes = bytes.subspan(count);
    }
    std::array<unsigned char, 32> digest{};
    Check(BCryptFinishHash(hash.h, digest.data(), static_cast<ULONG>(digest.size()), 0));
    constexpr wchar_t digits[] = L"0123456789abcdef";
    std::wstring result; result.reserve(64);
    for (auto value : digest) { result.push_back(digits[value >> 4]); result.push_back(digits[value & 15]); }
    return result;
}
}
