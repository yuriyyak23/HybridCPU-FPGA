#pragma once

namespace HybridCPU_ISE::CloseToHSL::Memory::Timing {
    // Identity is local to the memory controller; zero is invalid.
    struct MemoryRequestId final {
        unsigned long long Value = 0;
        constexpr MemoryRequestId() noexcept = default;
        explicit constexpr MemoryRequestId(unsigned long long value) noexcept : Value(value) {}
        constexpr bool getIsValid() const noexcept { return Value != 0; }
        constexpr bool isValid() const noexcept { return getIsValid(); }
        friend constexpr bool operator==(MemoryRequestId, MemoryRequestId) = default;
    };
}