#pragma once

#include "../../../../Runtime/Capabilities/Grants/CapabilityGrant.h"
#include "../../Frontend/Decode/VmxInstructionPayload.h"
#include <unordered_set>
#include <vector>
#include <span>
#include <string>
#include <utility>
#include <memory>
#include "../../../../../../tangible_string_helper.h"

namespace YAKSys_Hybrid_CPU::Core
{
	struct CapabilityBitSchemaEntry final {
    std::wstring Name; unsigned long long CapabilityMask; CapabilityGrantScope ProjectionScope; std::wstring TypedGrantSource; CapabilityFrontendProjectionPolicy FrontendProjectionPolicy; CapabilityEvidenceVisibility EvidenceVisibility;
    CapabilityBitSchemaEntry(std::wstring name, unsigned long long mask, CapabilityGrantScope scope, std::wstring source, CapabilityFrontendProjectionPolicy policy, CapabilityEvidenceVisibility evidence) : Name(std::move(name)), CapabilityMask(mask), ProjectionScope(scope), TypedGrantSource(std::move(source)), FrontendProjectionPolicy(policy), EvidenceVisibility(evidence) {}
};
struct CapabilityBitSchemaArtifact final {
    std::wstring SchemaName; int SchemaVersion; unsigned long long SourceHash, ArtifactHash, CompatibilityPublicationMask; int EntryCount; bool IsCanonical;
    CapabilityBitSchemaArtifact(std::wstring name, int version, unsigned long long source, unsigned long long artifact, unsigned long long mask, int count, bool canonical) : SchemaName(std::move(name)), SchemaVersion(version), SourceHash(source), ArtifactHash(artifact), CompatibilityPublicationMask(mask), EntryCount(count), IsCanonical(canonical) {}
};
class CapabilityDescriptorSetSchema final : public std::enable_shared_from_this<CapabilityDescriptorSetSchema>
	{
	private:
		static const std::shared_ptr<CapabilityDescriptorSetSchema> VmxCompatibility;
		static const std::shared_ptr<CapabilityBitSchemaArtifact> VmxCompatibilityBitSchema;
		const unsigned long long CompatibilityPublicationMask;

	public:
		CapabilityDescriptorSetSchema();

		CapabilityDescriptorSetSchema(unsigned long long compatibilityPublicationMask);

		static constexpr int CurrentVersion = 1;

		static unsigned long long getKnownVmxV2CompatibilityMask();

	private:
		static inline const std::vector<std::shared_ptr<CapabilityBitSchemaEntry>> VmxCompatibilityBitTable = {std::make_shared<CapabilityBitSchemaEntry>(L"VmPtrSt", VmxV2InstructionCaps::VmPtrSt, CapabilityGrantScope::CompatibilityProjection, L"CapabilityGrantCollection.TypedGrant", CapabilityFrontendProjectionPolicy::ProjectIfCompatible, CapabilityEvidenceVisibility::GuestVisibleProjection), std::make_shared<CapabilityBitSchemaEntry>(L"VmCall", VmxV2InstructionCaps::VmCall, CapabilityGrantScope::CompatibilityProjection, L"CapabilityGrantCollection.TypedGrant", CapabilityFrontendProjectionPolicy::ProjectIfCompatible, CapabilityEvidenceVisibility::GuestVisibleProjection), std::make_shared<CapabilityBitSchemaEntry>(L"Invept", VmxV2InstructionCaps::Invept, CapabilityGrantScope::CompatibilityProjection, L"CapabilityGrantCollection.TypedGrant", CapabilityFrontendProjectionPolicy::ProjectIfCompatible, CapabilityEvidenceVisibility::GuestVisibleProjection), std::make_shared<CapabilityBitSchemaEntry>(L"Invvpid", VmxV2InstructionCaps::Invvpid, CapabilityGrantScope::CompatibilityProjection, L"CapabilityGrantCollection.TypedGrant", CapabilityFrontendProjectionPolicy::ProjectIfCompatible, CapabilityEvidenceVisibility::GuestVisibleProjection), std::make_shared<CapabilityBitSchemaEntry>(L"VmFunc", VmxV2InstructionCaps::VmFunc, CapabilityGrantScope::CompatibilityProjection, L"CapabilityGrantCollection.TypedGrant", CapabilityFrontendProjectionPolicy::ProjectIfCompatible, CapabilityEvidenceVisibility::GuestVisibleProjection), std::make_shared<CapabilityBitSchemaEntry>(L"RootDescriptorOperand", VmxV2InstructionCaps::RootDescriptorOperand, CapabilityGrantScope::CompatibilityProjection, L"CapabilityGrantCollection.TypedGrant", CapabilityFrontendProjectionPolicy::ProjectIfCompatible, CapabilityEvidenceVisibility::GuestVisibleProjection), std::make_shared<CapabilityBitSchemaEntry>(L"VmSaveX", VmxV2InstructionCaps::VmSaveX, CapabilityGrantScope::CompatibilityProjection, L"CapabilityGrantCollection.TypedGrant", CapabilityFrontendProjectionPolicy::ProjectIfCompatible, CapabilityEvidenceVisibility::GuestVisibleProjection), std::make_shared<CapabilityBitSchemaEntry>(L"VmRestX", VmxV2InstructionCaps::VmRestX, CapabilityGrantScope::CompatibilityProjection, L"CapabilityGrantCollection.TypedGrant", CapabilityFrontendProjectionPolicy::ProjectIfCompatible, CapabilityEvidenceVisibility::GuestVisibleProjection), std::make_shared<CapabilityBitSchemaEntry>(L"NestedVmx", VmxV2InstructionCaps::NestedVmx, CapabilityGrantScope::CompatibilityProjection, L"NestedDomainCapability.TypedGrant", CapabilityFrontendProjectionPolicy::ProjectIfCompatible, CapabilityEvidenceVisibility::GuestVisibleProjection)};

	public:
		static std::shared_ptr<CapabilityDescriptorSetSchema> getVmxCompatibility();

		static std::span<const std::shared_ptr<CapabilityBitSchemaEntry>> getVmxCompatibilityBits();

		static std::shared_ptr<CapabilityBitSchemaArtifact> getVmxCompatibilityBitSchema();

		unsigned long long getCompatibilityPublicationMask() const;

		unsigned long long FilterCompatibilityCaps(unsigned long long capabilityMask);

		unsigned long long GetUnknownCompatibilityCaps(unsigned long long capabilityMask);

		bool ContainsOnlyKnownCompatibilityCaps(unsigned long long capabilityMask);

		bool MatchesCanonicalVmxCompatibilityBitSchema(std::shared_ptr<CapabilityBitSchemaArtifact> artifact);






	private:

	public:













	private:

	public:







	};
}
