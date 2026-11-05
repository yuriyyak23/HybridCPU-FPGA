#pragma once
#include <algorithm>
#include <stdexcept>
#include <vector>

namespace YAKSys_Hybrid_CPU::Core::Decoder {
inline std::vector<int> FreezeRegisterIds(const std::vector<int> &registers) {
    if (std::any_of(registers.begin(), registers.end(), [](int value){ return value < 0; }))
        throw std::out_of_range("Register identifiers cannot be negative.");
    return registers;
}
}
