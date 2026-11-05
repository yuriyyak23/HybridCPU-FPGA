#pragma once
#include <array>
#include <cstdint>
#include <string>
#include "tangible_string_helper.h"
namespace HybridCpu::ShadowFrontend {
struct DecodeContext final {
    std::wstring ManifestVersion, ManifestHash, Extensions, DecoderEpoch, DecoderVersion, Privilege, Domain, AddressSpace, VectorConfiguration;
    std::uint64_t ExecutableEpoch = 0, CodeEpoch = 0;
    bool ReplayEligible = false;
    std::wstring ContextIncarnation, SidebandIdentity;
    bool isBound() const {
        if (!ReplayEligible) return false;
        const std::array<const std::wstring*, 11> values{&ManifestVersion, &ManifestHash, &Extensions, &DecoderEpoch, &DecoderVersion, &Privilege, &Domain, &AddressSpace, &VectorConfiguration, &ContextIncarnation, &SidebandIdentity};
        for (const auto* value : values) if (StringHelper::isEmptyOrWhiteSpace(*value) || *value == L"unbound") return false;
        return true;
    }
    bool operator==(const DecodeContext&) const = default;
};
}
