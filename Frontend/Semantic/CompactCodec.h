#pragma once
#include "../Contracts/Contracts.h"
#include <memory>
#include <optional>
#include <span>
#include <vector>
namespace HybridCpu::ShadowFrontend {
// Bounded v1 lossless snapshot codec; implementation must retain Brotli compatibility.
class CompactCodec final {
private:
    static constexpr unsigned int Magic = 0x31434244;

public:
    static constexpr int MaxDecodedBytes = 131072;
    static constexpr int MaxEncodedBytes = 131088;
    static std::optional<std::vector<unsigned char>> Encode(std::shared_ptr<DecodeResult> result);
    static bool TryDecode(std::span<const unsigned char> encoded, std::shared_ptr<DecodeResult>& result);
};
}
