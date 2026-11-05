#include "CanonicalPayloadSnapshot.h"
#include "DeclarativeDecoderStages.h"
#include "../Pipeline/MicroOps/IR/InstructionIR.h"
#include "../ISA/Instructions/NonVmx/Lanes00_03Vector/MatrixTile/MatrixTileNumericPolicyAbi.h"
#include "../ISA/Instructions/NonVmx/Lanes00_03Vector/MatrixTile/MatrixTileLayoutPolicyAbi.h"
#include "../Pipeline/Metadata/BundleMetadata.h"
#include "../Pipeline/Metadata/MicroOpAdmissionMetadata.h"
#include "../Contracts/CompilerTransport/InstructionSlotMetadata.h"
#include "../Execution/ExternalAccelerators/Descriptors/AcceleratorCommandDescriptor.h"
#include "Sha256Digest.h"
#include <stdexcept>
#include <algorithm>
#include <cwctype>
#include <functional>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core::Decoder {
namespace {
using VectorPayload = YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps::VectorInstructionPayload;
using NumericPolicy = YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile::MatrixTileNumericPolicy;
using LayoutPolicy = YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile::MatrixTileLayoutPolicy;

std::shared_ptr<DeclarativeInstructionSemantics> CloneDeclarativeSemantics(
    const DeclarativeInstructionSemantics& source) {
    std::optional<std::shared_ptr<VectorPayload>> clonedVectorPayload;
    if (source.VectorPayload.has_value()) {
        const auto& sourceVectorPayload = source.VectorPayload.value();
        if (!sourceVectorPayload) {
            clonedVectorPayload.emplace(nullptr);
        } else {
            auto vectorPayload = std::make_shared<VectorPayload>(*sourceVectorPayload);
            if (sourceVectorPayload->MatrixTileNumericPolicy) {
                vectorPayload->MatrixTileNumericPolicy = std::make_shared<NumericPolicy>(*sourceVectorPayload->MatrixTileNumericPolicy);
            }
            if (sourceVectorPayload->MatrixTileLayoutPolicy) {
                vectorPayload->MatrixTileLayoutPolicy = std::make_shared<LayoutPolicy>(*sourceVectorPayload->MatrixTileLayoutPolicy);
            }
            clonedVectorPayload.emplace(std::move(vectorPayload));
        }
    }

    return std::make_shared<DeclarativeInstructionSemantics>(
        source.Opcode, source.InstructionClass, source.SerializationClass,
        source.Rd, source.Rs1, source.Rs2, source.Immediate,
        std::move(clonedVectorPayload));
}

std::shared_ptr<YAKSys_Hybrid_CPU::Core::BundleMetadata> CloneBundleMetadata(
    const YAKSys_Hybrid_CPU::Core::BundleMetadata& source) {
    using BundleMetadata = YAKSys_Hybrid_CPU::Core::BundleMetadata;
    using SlotMetadata = YAKSys_Hybrid_CPU::Core::SlotMetadata;
    auto clone = std::make_shared<BundleMetadata>();
    clone->SchemaVersion = source.SchemaVersion;
    clone->FspBoundary = source.FspBoundary;
    clone->BundleThermalHint = source.BundleThermalHint;
    clone->IsReplayAnchor = source.IsReplayAnchor;
    clone->DiagnosticsTag = source.DiagnosticsTag;

    if (source.SlotMetadata) {
        clone->SlotMetadata = std::make_shared<std::vector<std::shared_ptr<SlotMetadata>>>();
        clone->SlotMetadata->reserve(source.SlotMetadata->size());
        for (const auto& sourceSlot : *source.SlotMetadata) {
            if (!sourceSlot) {
                clone->SlotMetadata->push_back(nullptr);
                continue;
            }
            auto slot = std::make_shared<SlotMetadata>(*sourceSlot);
            if (sourceSlot->AdmissionMetadata) {
                slot->AdmissionMetadata = std::make_shared<YAKSys_Hybrid_CPU::Core::MicroOpAdmissionMetadata>(
                    *sourceSlot->AdmissionMetadata);
            }
            clone->SlotMetadata->push_back(std::move(slot));
        }
    }
    return clone;
}

std::shared_ptr<HybridCPU_ISE::Arch::InstructionSlotMetadata> CloneInstructionSlotMetadata(
    const HybridCPU_ISE::Arch::InstructionSlotMetadata& source) {
    auto clone = std::make_shared<HybridCPU_ISE::Arch::InstructionSlotMetadata>();
    clone->VirtualThreadId = source.VirtualThreadId;
    if (source.SlotMetadata) {
        clone->SlotMetadata = std::make_shared<YAKSys_Hybrid_CPU::Core::SlotMetadata>(*source.SlotMetadata);
        if (source.SlotMetadata->AdmissionMetadata)
            clone->SlotMetadata->AdmissionMetadata = std::make_shared<YAKSys_Hybrid_CPU::Core::MicroOpAdmissionMetadata>(*source.SlotMetadata->AdmissionMetadata);
    }
    if (source.DmaStreamComputeDescriptor)
        clone->DmaStreamComputeDescriptor = std::make_shared<YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute::DmaStreamComputeDescriptor>(*source.DmaStreamComputeDescriptor);
    if (source.AcceleratorCommandDescriptor)
        clone->AcceleratorCommandDescriptor = std::make_shared<YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors::AcceleratorCommandDescriptor>(*source.AcceleratorCommandDescriptor);
    if (source.MatrixTileNumericPolicy)
        clone->MatrixTileNumericPolicy = std::make_shared<NumericPolicy>(*source.MatrixTileNumericPolicy);
    if (source.MatrixTileLayoutPolicy)
        clone->MatrixTileLayoutPolicy = std::make_shared<LayoutPolicy>(*source.MatrixTileLayoutPolicy);
    return clone;
}
}

CanonicalPayloadSnapshot::CanonicalPayloadSnapshot(std::wstring kind, std::vector<unsigned char> content, std::wstring hash)
    : Kind(std::move(kind)), ContentSha256(std::move(hash)), content_(std::move(content)) {}
std::wstring CanonicalPayloadSnapshot::getKind() const { return Kind; }
std::wstring CanonicalPayloadSnapshot::getContentSha256() const { return ContentSha256; }
std::vector<unsigned char> CanonicalPayloadSnapshot::getContent() const { return content_; }
std::shared_ptr<CanonicalPayloadSnapshot> CanonicalPayloadSnapshot::FromBytes(const std::wstring &kind, std::span<const unsigned char> content) {
    if (kind.empty() || std::all_of(kind.begin(), kind.end(), [](wchar_t c) { return std::iswspace(c) != 0; }))
        throw std::invalid_argument("kind must not be empty or whitespace");
    std::vector<unsigned char> copy(content.begin(), content.end());
    auto hash = Hash(copy);
    return std::shared_ptr<CanonicalPayloadSnapshot>(new CanonicalPayloadSnapshot(kind, std::move(copy), std::move(hash)));
}
std::shared_ptr<CanonicalPayloadSnapshot> CanonicalPayloadSnapshot::FromBytes(const std::wstring &kind, std::span<const unsigned char> content, std::any frozenObject) {
    if (kind == L"DeclarativeInstructionSemantics") {
        auto source = std::any_cast<std::shared_ptr<DeclarativeInstructionSemantics>>(&frozenObject);
        if (!source || !*source)
            throw std::invalid_argument("frozenObject must contain shared_ptr<DeclarativeInstructionSemantics>");
        auto snapshot = FromBytes(kind, content);
        snapshot->frozenDeclarativeSemantics_ = CloneDeclarativeSemantics(**source);
        return snapshot;
    }
    if (kind == L"BundleMetadata") {
        auto source = std::any_cast<std::shared_ptr<YAKSys_Hybrid_CPU::Core::BundleMetadata>>(&frozenObject);
        if (!source || !*source)
            throw std::invalid_argument("frozenObject must contain non-null shared_ptr<BundleMetadata>");
        auto snapshot = FromBytes(kind, content);
        snapshot->frozenBundleMetadata_ = CloneBundleMetadata(**source);
        return snapshot;
    }
    if (kind == L"InstructionSlotMetadata") {
        auto source = std::any_cast<std::shared_ptr<HybridCPU_ISE::Arch::InstructionSlotMetadata>>(&frozenObject);
        if (!source || !*source) throw std::invalid_argument("frozenObject must contain non-null shared_ptr<InstructionSlotMetadata>");
        auto snapshot = FromBytes(kind, content);
        snapshot->frozenInstructionSlotMetadata_ = CloneInstructionSlotMetadata(**source);
        return snapshot;
    }
    throw std::invalid_argument("typed frozen payload kind is unsupported");
}
std::wstring CanonicalPayloadSnapshot::Hash(std::span<const unsigned char> content) { return Sha256Hex(content); }
bool CanonicalPayloadSnapshot::Equals(std::shared_ptr<CanonicalPayloadSnapshot> other) const {
    return other && Kind == other->Kind && ContentSha256 == other->ContentSha256 && content_ == other->content_;
}
bool CanonicalPayloadSnapshot::Equals(std::any obj) const {
    const auto *other = std::any_cast<std::shared_ptr<CanonicalPayloadSnapshot>>(&obj);
    return other && Equals(*other);
}
int CanonicalPayloadSnapshot::GetHashCode() const {
    const auto a = std::hash<std::wstring>{}(Kind), b = std::hash<std::wstring>{}(ContentSha256);
    return static_cast<int>((a ^ (b + 0x9e3779b9U + (a << 6) + (a >> 2))) & 0x7fffffffU);
}

template<>
std::optional<std::shared_ptr<DeclarativeInstructionSemantics>>
CanonicalPayloadSnapshot::Deserialize<std::shared_ptr<DeclarativeInstructionSemantics>>() {
    if (!frozenDeclarativeSemantics_) return std::nullopt;
    return CloneDeclarativeSemantics(*frozenDeclarativeSemantics_);
}

template<>
std::optional<std::shared_ptr<YAKSys_Hybrid_CPU::Core::BundleMetadata>>
CanonicalPayloadSnapshot::Deserialize<std::shared_ptr<YAKSys_Hybrid_CPU::Core::BundleMetadata>>() {
    if (!frozenBundleMetadata_) return std::nullopt;
    return CloneBundleMetadata(*frozenBundleMetadata_);
}

template<>
std::optional<std::shared_ptr<HybridCPU_ISE::Arch::InstructionSlotMetadata>>
CanonicalPayloadSnapshot::Deserialize<std::shared_ptr<HybridCPU_ISE::Arch::InstructionSlotMetadata>>() {
    if (!frozenInstructionSlotMetadata_) return std::nullopt;
    return CloneInstructionSlotMetadata(*frozenInstructionSlotMetadata_);
}
}
