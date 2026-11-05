#pragma once
#include <limits>
#include <stdexcept>
#include <string>
namespace YAKSys_Hybrid_CPU::Memory {
class MemoryBankGeometryGeneration final {
    unsigned long long value_ = 0;
public:
    static constexpr unsigned long long MinValue = 1, MaxValue = std::numeric_limits<unsigned long long>::max();
    MemoryBankGeometryGeneration() = default;
    explicit MemoryBankGeometryGeneration(unsigned long long value) : value_(value) { if (!IsRepresentable(value)) throw std::out_of_range("generation"); }
    unsigned long long getValue() const noexcept { return value_; }
    bool isIssued() const noexcept { return IsRepresentable(value_); }
    static bool IsRepresentable(unsigned long long value) noexcept { return value >= MinValue; }
    static MemoryBankGeometryGeneration FromRawValue(unsigned long long value) { return MemoryBankGeometryGeneration(value); }
    unsigned long long ToRawValue() const noexcept { return value_; }
    static MemoryBankGeometryGeneration Create(unsigned long long value) { return MemoryBankGeometryGeneration(value); }
    static bool TryCreate(unsigned long long value, MemoryBankGeometryGeneration& result) { if (!IsRepresentable(value)) { result = {}; return false; } result = MemoryBankGeometryGeneration(value); return true; }
    std::wstring ToString() const { return isIssued() ? L"memory-bank-geometry-generation" + std::to_wstring(value_) : L"unissued"; }
    bool operator==(const MemoryBankGeometryGeneration&) const = default;
};
}
