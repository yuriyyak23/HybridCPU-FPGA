#pragma once
#include <cstdint>

namespace YAKSys_Hybrid_CPU::Execution::Detail {
    inline std::uint64_t MultiplyHighUnsigned64(std::uint64_t lhs, std::uint64_t rhs) noexcept {
        const std::uint64_t lhsLow = static_cast<std::uint32_t>(lhs);
        const std::uint64_t lhsHigh = lhs >> 32;
        const std::uint64_t rhsLow = static_cast<std::uint32_t>(rhs);
        const std::uint64_t rhsHigh = rhs >> 32;
        std::uint64_t product = lhsLow * rhsLow;
        const std::uint64_t carry = product >> 32;
        product = lhsHigh * rhsLow + carry;
        const std::uint64_t middle = static_cast<std::uint32_t>(product);
        const std::uint64_t high = product >> 32;
        product = lhsLow * rhsHigh + middle;
        return lhsHigh * rhsHigh + high + (product >> 32);
    }

    inline std::uint64_t MultiplyHighSignedUnsigned64(std::uint64_t signedLhs, std::uint64_t unsignedRhs) noexcept {
        const std::uint64_t unsignedHigh = MultiplyHighUnsigned64(signedLhs, unsignedRhs);
        return (signedLhs >> 63) ? unsignedHigh - unsignedRhs : unsignedHigh;
    }

    inline std::uint64_t MultiplyHighSigned64(std::uint64_t signedLhs, std::uint64_t signedRhs) noexcept {
        std::uint64_t high = MultiplyHighUnsigned64(signedLhs, signedRhs);
        if (signedLhs >> 63) high -= signedRhs;
        if (signedRhs >> 63) high -= signedLhs;
        return high;
    }
}
