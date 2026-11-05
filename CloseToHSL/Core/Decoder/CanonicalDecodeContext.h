#pragma once

#include <memory>
#include <string>

namespace YAKSys_Hybrid_CPU::Core::Decoder {
// C# record used as immutable replay identity input. The builder owns these
// values until publication; consumers hold a shared snapshot.
struct CanonicalDecodeContext final {
    std::wstring ManifestVersion;
    std::wstring ManifestHash;
    std::wstring ExtensionConfigurationFingerprint;
    std::wstring DecoderEpoch;
    std::wstring DecoderVersion;
    std::wstring PrivilegeContext;
    std::wstring DomainIdentity;
    std::wstring AddressSpaceIdentity;
    std::wstring VectorConfigurationFingerprint;
    unsigned long long ExecutableMemoryInvalidationEpoch = 0;
    unsigned long long CodeGenerationEpoch = 0;
    bool IsReplayEligible = false;

    bool operator==(const CanonicalDecodeContext&) const = default;

    static const std::shared_ptr<CanonicalDecodeContext> Unbound;
};
}
