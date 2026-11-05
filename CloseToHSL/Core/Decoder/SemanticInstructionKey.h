#pragma once
#include "CanonicalDecodeContext.h"
#include <any>
#include <span>
#include <vector>

namespace YAKSys_Hybrid_CPU::Core::Decoder {
class SemanticInstructionKey {
    std::vector<unsigned char> bytes_;
    std::wstring rawHash_, annotationsHash_;
    CanonicalDecodeContext context_;
public:
    SemanticInstructionKey() = default;
    static SemanticInstructionKey Create(std::span<const unsigned char> bytes, const std::wstring &annotations, std::shared_ptr<CanonicalDecodeContext> context);
    std::wstring getRawBundleSha256() const { return rawHash_; }
    std::wstring getAnnotationsSha256() const { return annotationsHash_; }
    std::wstring getManifestVersion() const { return context_.ManifestVersion; }
    std::wstring getManifestHash() const { return context_.ManifestHash; }
    std::wstring getExtensionConfigurationFingerprint() const { return context_.ExtensionConfigurationFingerprint; }
    std::wstring getDecoderEpoch() const { return context_.DecoderEpoch; }
    std::wstring getDecoderVersion() const { return context_.DecoderVersion; }
    std::wstring getPrivilegeContext() const { return context_.PrivilegeContext; }
    std::wstring getDomainIdentity() const { return context_.DomainIdentity; }
    std::wstring getAddressSpaceIdentity() const { return context_.AddressSpaceIdentity; }
    std::wstring getVectorConfigurationFingerprint() const { return context_.VectorConfigurationFingerprint; }
    unsigned long long getExecutableMemoryInvalidationEpoch() const { return context_.ExecutableMemoryInvalidationEpoch; }
    unsigned long long getCodeGenerationEpoch() const { return context_.CodeGenerationEpoch; }
    bool isReplayEligible() const { return context_.IsReplayEligible; }
    std::vector<unsigned char> getRawBundleBytes() const { return bytes_; }
    bool Equals(const SemanticInstructionKey &other) const;
    bool Equals(std::any obj) const;
    int GetHashCode() const;
    bool operator==(const SemanticInstructionKey &other) const { return Equals(other); }
    bool operator!=(const SemanticInstructionKey &other) const { return !Equals(other); }
};
}
