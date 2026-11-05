#pragma once
#include <span>
#include <vector>
namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute {
class DmaStreamComputeStagedWrite final {
public:
    const unsigned long long Address;
    const std::vector<unsigned char> Data;
    const unsigned long long Length;
    DmaStreamComputeStagedWrite(unsigned long long address, std::span<const unsigned char> data)
        : Address(address), Data(data.begin(), data.end()), Length(Data.size()) {}
};
}
