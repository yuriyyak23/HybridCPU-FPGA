#pragma once
#include <span>
#include <string>

namespace YAKSys_Hybrid_CPU::Core::Decoder {
// Returns the lowercase SHA-256 digest of the exact byte sequence.
std::wstring Sha256Hex(std::span<const unsigned char> bytes);
}
