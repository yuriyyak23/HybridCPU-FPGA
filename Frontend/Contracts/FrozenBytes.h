#pragma once
#include <any>
#include <memory>
#include <span>
#include <vector>
namespace HybridCpu::ShadowFrontend {
class FrozenBytes final {
    const std::vector<unsigned char> bytes;
public:
    explicit FrozenBytes(std::span<const unsigned char> value) : bytes(value.begin(), value.end()) {}
    int getLength() const { return static_cast<int>(bytes.size()); }
    std::vector<unsigned char> Copy() const { return bytes; }
    std::span<const unsigned char> getSpan() const noexcept { return bytes; }
    bool Equals(const std::shared_ptr<FrozenBytes>& other) const { return other && bytes == other->bytes; }
    bool Equals(const std::any& obj) const {
        const auto value = std::any_cast<std::shared_ptr<FrozenBytes>>(&obj);
        return value && Equals(*value);
    }
    int GetHashCode() const { return getLength(); }
};
}
