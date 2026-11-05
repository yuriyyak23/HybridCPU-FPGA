#pragma once
#include <any>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <vector>

namespace YAKSys_Hybrid_CPU::Core { class BundleMetadata; }
namespace HybridCPU_ISE::Arch { class InstructionSlotMetadata; }

namespace YAKSys_Hybrid_CPU::Core::Decoder {
struct DeclarativeInstructionSemantics;
class CanonicalPayloadSnapshot final {
    const std::wstring Kind;
    const std::wstring ContentSha256;
    const std::vector<unsigned char> content_;
    std::shared_ptr<const DeclarativeInstructionSemantics> frozenDeclarativeSemantics_;
    std::shared_ptr<const YAKSys_Hybrid_CPU::Core::BundleMetadata> frozenBundleMetadata_;
    std::shared_ptr<const HybridCPU_ISE::Arch::InstructionSlotMetadata> frozenInstructionSlotMetadata_;
    CanonicalPayloadSnapshot(std::wstring kind, std::vector<unsigned char> content, std::wstring hash);
public:
    std::wstring getKind() const;
    std::wstring getContentSha256() const;
    // An owning copy preserves the source's read-only-memory lifetime semantics.
    std::vector<unsigned char> getContent() const;
    static std::shared_ptr<CanonicalPayloadSnapshot> FromObject(const std::wstring &kind, std::any value);
    static std::shared_ptr<CanonicalPayloadSnapshot> FromBytes(const std::wstring &kind, std::span<const unsigned char> content);
    static std::shared_ptr<CanonicalPayloadSnapshot> FromBytes(const std::wstring &kind, std::span<const unsigned char> content, std::any frozenObject);
    static std::wstring Hash(std::span<const unsigned char> content);
    template<typename T> std::optional<T> Deserialize();
    bool Equals(std::shared_ptr<CanonicalPayloadSnapshot> other) const;
    bool Equals(std::any obj) const;
    int GetHashCode() const;
};

template<>
std::optional<std::shared_ptr<DeclarativeInstructionSemantics>>
CanonicalPayloadSnapshot::Deserialize<std::shared_ptr<DeclarativeInstructionSemantics>>();

template<>
std::optional<std::shared_ptr<YAKSys_Hybrid_CPU::Core::BundleMetadata>>
CanonicalPayloadSnapshot::Deserialize<std::shared_ptr<YAKSys_Hybrid_CPU::Core::BundleMetadata>>();

template<>
std::optional<std::shared_ptr<HybridCPU_ISE::Arch::InstructionSlotMetadata>>
CanonicalPayloadSnapshot::Deserialize<std::shared_ptr<HybridCPU_ISE::Arch::InstructionSlotMetadata>>();
}
