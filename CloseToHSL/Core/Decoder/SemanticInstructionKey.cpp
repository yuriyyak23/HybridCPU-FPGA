#include "SemanticInstructionKey.h"
#include "Sha256Digest.h"
#include <algorithm>
#include <cwctype>
#include <functional>
#include <stdexcept>
#include <tuple>

namespace YAKSys_Hybrid_CPU::Core::Decoder {
namespace {
bool Blank(const std::wstring &s) { return s.empty() || std::all_of(s.begin(), s.end(), [](wchar_t c){ return std::iswspace(c) != 0; }); }
auto Factors(const CanonicalDecodeContext &c) {
    return std::tie(c.ManifestVersion, c.ManifestHash, c.ExtensionConfigurationFingerprint, c.DecoderEpoch,
        c.DecoderVersion, c.PrivilegeContext, c.DomainIdentity, c.AddressSpaceIdentity,
        c.VectorConfigurationFingerprint, c.ExecutableMemoryInvalidationEpoch, c.CodeGenerationEpoch, c.IsReplayEligible);
}
}
SemanticInstructionKey SemanticInstructionKey::Create(std::span<const unsigned char> bytes, const std::wstring &annotations, std::shared_ptr<CanonicalDecodeContext> context) {
    if (Blank(annotations) || !context) throw std::invalid_argument("annotations and context must be supplied");
    if (Blank(context->ManifestVersion) || Blank(context->ManifestHash) || Blank(context->ExtensionConfigurationFingerprint) ||
        Blank(context->DecoderEpoch) || Blank(context->DecoderVersion) || Blank(context->PrivilegeContext) ||
        Blank(context->DomainIdentity) || Blank(context->AddressSpaceIdentity) || Blank(context->VectorConfigurationFingerprint))
        throw std::invalid_argument("Every SemanticInstructionKey context factor must be explicit.");
    SemanticInstructionKey result; result.bytes_.assign(bytes.begin(), bytes.end()); result.rawHash_ = Sha256Hex(result.bytes_);
    result.annotationsHash_ = annotations; result.context_ = *context; return result;
}
bool SemanticInstructionKey::Equals(const SemanticInstructionKey &other) const {
    return rawHash_ == other.rawHash_ && annotationsHash_ == other.annotationsHash_ && Factors(context_) == Factors(other.context_) && bytes_ == other.bytes_;
}
bool SemanticInstructionKey::Equals(std::any obj) const { const auto *other = std::any_cast<SemanticInstructionKey>(&obj); return other && Equals(*other); }
int SemanticInstructionKey::GetHashCode() const {
    // Equal keys have equal raw and annotation hashes; unequal keys may collide.
    auto a=std::hash<std::wstring>{}(rawHash_), b=std::hash<std::wstring>{}(annotationsHash_);
    return static_cast<int>((a ^ (b + 0x9e3779b9U + (a << 6) + (a >> 2))) & 0x7fffffffU);
}
}
