#pragma once
#include <limits>
#include <stdexcept>
#include <cstdint>
namespace HybridCpu::ShadowFrontend {
struct BundleRequest final {
    std::uint64_t Pc = 0;
    int VirtualThread = 0;
    std::uint64_t TransportSerial = 0;
    int Cursor = 0;
    void Validate() const {
        if (VirtualThread < 0 || VirtualThread > 3 || Cursor < 0 || Cursor > 8 || Pc > (std::numeric_limits<std::uint64_t>::max)() - 255)
            throw std::out_of_range("BundleRequest");
    }
    bool operator==(const BundleRequest&) const = default;
};
struct FreshnessToken final {
    std::uint64_t Incarnation = 0;
    std::uint64_t Generation = 0;
    bool operator==(const FreshnessToken&) const = default;
};
}
