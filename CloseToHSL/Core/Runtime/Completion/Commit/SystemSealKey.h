#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#include <bcrypt.h>
#include <stdexcept>
#include <vector>
#pragma comment(lib, "bcrypt.lib")
namespace YAKSys_Hybrid_CPU::Core {
inline std::vector<unsigned char> GenerateSystemSealKey() {
    std::vector<unsigned char> key(32);
    if (BCryptGenRandom(nullptr, key.data(), static_cast<ULONG>(key.size()), BCRYPT_USE_SYSTEM_PREFERRED_RNG) < 0)
        throw std::runtime_error("Failed to generate completion receipt seal key.");
    return key;
}
}
