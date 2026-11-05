#pragma once
#include <limits>
#include <stdexcept>
#include <string>
namespace YAKSys_Hybrid_CPU::Memory {
class PhysicalMemoryBankIndex final {
    int value_ = 0;
public:
    static constexpr int MinValue = 0, MaxValue = std::numeric_limits<int>::max();
    PhysicalMemoryBankIndex() = default;
    explicit PhysicalMemoryBankIndex(int value) : value_(value) { if (!IsRepresentable(value)) throw std::out_of_range("bank index"); }
    int getValue() const noexcept { return value_; }
    static bool IsRepresentable(int value) noexcept { return value >= MinValue; }
    static PhysicalMemoryBankIndex FromRawValue(int value) { return PhysicalMemoryBankIndex(value); }
    int ToRawValue() const noexcept { return value_; }
    static PhysicalMemoryBankIndex Create(int value) { return PhysicalMemoryBankIndex(value); }
    static bool TryCreate(int value, PhysicalMemoryBankIndex& result) { if (!IsRepresentable(value)) { result = {}; return false; } result = PhysicalMemoryBankIndex(value); return true; }
    std::wstring ToString() const { return L"physical-bank" + std::to_wstring(value_); }
    operator int() const noexcept { return value_; }
    bool operator==(const PhysicalMemoryBankIndex&) const = default;
    static const PhysicalMemoryBankIndex Zero;
};
inline const PhysicalMemoryBankIndex PhysicalMemoryBankIndex::Zero{};
}
