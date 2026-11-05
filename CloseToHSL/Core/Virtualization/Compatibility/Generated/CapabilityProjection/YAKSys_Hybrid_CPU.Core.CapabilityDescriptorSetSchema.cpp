#include "YAKSys_Hybrid_CPU.Core.CapabilityDescriptorSetSchema.h"

namespace YAKSys_Hybrid_CPU::Core
{
    const std::shared_ptr<CapabilityDescriptorSetSchema> CapabilityDescriptorSetSchema::VmxCompatibility = std::make_shared<CapabilityDescriptorSetSchema>(getKnownVmxV2CompatibilityMask());
    const std::shared_ptr<CapabilityBitSchemaArtifact> CapabilityDescriptorSetSchema::VmxCompatibilityBitSchema = std::make_shared<CapabilityBitSchemaArtifact>(L"vmxcaps-capability-bit-schema", CurrentVersion, 0x6A4D2C781B90E317ULL, 0xB781C25E9D4A603FULL, getKnownVmxV2CompatibilityMask(), static_cast<int>(VmxCompatibilityBitTable.size()), true);

	CapabilityDescriptorSetSchema::CapabilityDescriptorSetSchema() : CapabilityDescriptorSetSchema(0)
	{
	}

	CapabilityDescriptorSetSchema::CapabilityDescriptorSetSchema(unsigned long long compatibilityPublicationMask) : CompatibilityPublicationMask(compatibilityPublicationMask) {}

	unsigned long long CapabilityDescriptorSetSchema::getKnownVmxV2CompatibilityMask()
	{
		return VmxV2InstructionCaps::VmPtrSt | VmxV2InstructionCaps::VmCall | VmxV2InstructionCaps::Invept | VmxV2InstructionCaps::Invvpid | VmxV2InstructionCaps::VmFunc | VmxV2InstructionCaps::RootDescriptorOperand | VmxV2InstructionCaps::VmSaveX | VmxV2InstructionCaps::VmRestX | VmxV2InstructionCaps::NestedVmx;
	}

	std::shared_ptr<CapabilityDescriptorSetSchema> CapabilityDescriptorSetSchema::getVmxCompatibility()
	{
		return VmxCompatibility;
	}

	std::span<const std::shared_ptr<CapabilityBitSchemaEntry>> CapabilityDescriptorSetSchema::getVmxCompatibilityBits()
	{
		return VmxCompatibilityBitTable;
	}

	std::shared_ptr<CapabilityBitSchemaArtifact> CapabilityDescriptorSetSchema::getVmxCompatibilityBitSchema()
	{
		return VmxCompatibilityBitSchema;
	}

	unsigned long long CapabilityDescriptorSetSchema::getCompatibilityPublicationMask() const
	{
		return CompatibilityPublicationMask;
	}

	unsigned long long CapabilityDescriptorSetSchema::FilterCompatibilityCaps(unsigned long long capabilityMask)
	{
		return capabilityMask & getCompatibilityPublicationMask();
	}

	unsigned long long CapabilityDescriptorSetSchema::GetUnknownCompatibilityCaps(unsigned long long capabilityMask)
	{
		return capabilityMask & ~getCompatibilityPublicationMask();
	}

	bool CapabilityDescriptorSetSchema::ContainsOnlyKnownCompatibilityCaps(unsigned long long capabilityMask)
	{
		return GetUnknownCompatibilityCaps(capabilityMask) == 0;
	}

	bool CapabilityDescriptorSetSchema::MatchesCanonicalVmxCompatibilityBitSchema(std::shared_ptr<CapabilityBitSchemaArtifact> artifact)
	{
		if (!artifact->IsCanonical || artifact->SchemaVersion != CurrentVersion || artifact->SourceHash == 0 || artifact->ArtifactHash == 0 || artifact->CompatibilityPublicationMask != getCompatibilityPublicationMask() || artifact->EntryCount != VmxCompatibilityBitTable.size())
		{
			return false;
		}

		unsigned long long declaredMask = 0;
		std::unordered_set<unsigned long long> seenMasks;

		for (auto entry : getVmxCompatibilityBits())
		{
			if (StringHelper::isEmptyOrWhiteSpace(entry->Name) || StringHelper::isEmptyOrWhiteSpace(entry->TypedGrantSource) || entry->CapabilityMask == 0 || entry->ProjectionScope != CapabilityGrantScope::CompatibilityProjection || entry->FrontendProjectionPolicy != CapabilityFrontendProjectionPolicy::ProjectIfCompatible || entry->EvidenceVisibility != CapabilityEvidenceVisibility::GuestVisibleProjection || !seenMasks.insert(entry->CapabilityMask).second)
			{
				return false;
			}

			declaredMask |= entry->CapabilityMask;
		}

		return declaredMask == artifact->CompatibilityPublicationMask;
	}
}
