#include "YAKSys_Hybrid_CPU.Core.VmxCapsBitSchemaConformanceContract.h"
#include "../../../Runtime/Capabilities/Grants/CapabilityGrant.h"

namespace YAKSys_Hybrid_CPU::Core
{

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public VmxCapsBitSchemaConformanceViolation Evaluate(CapabilityDescriptorSetSchema? schema, CapabilityBitSchemaArtifact artifact)
	VmxCapsBitSchemaConformanceViolation VmxCapsBitSchemaConformanceContract::Evaluate(std::shared_ptr<CapabilityDescriptorSetSchema> schema, std::shared_ptr<CapabilityBitSchemaArtifact> artifact)
	{
		if (schema == nullptr)
		{
			return VmxCapsBitSchemaConformanceViolation::MissingSchema;
		}

		if (!artifact->IsCanonical || artifact->EntryCount == 0 || artifact->SourceHash == 0 || artifact->ArtifactHash == 0)
		{
			return VmxCapsBitSchemaConformanceViolation::MissingCanonicalArtifact;
		}

		if (!schema->MatchesCanonicalVmxCompatibilityBitSchema(artifact))
		{
			return VmxCapsBitSchemaConformanceViolation::SchemaMismatch;
		}

		for (auto entry : CapabilityDescriptorSetSchema::getVmxCompatibilityBits())
		{
			if ((StringHelper::isEmptyOrWhiteSpace(entry->TypedGrantSource) || (entry->TypedGrantSource.find(L"TypedGrant") == std::wstring::npos && entry->TypedGrantSource.find(L"TypedGrants") == std::wstring::npos)))
			{
				return VmxCapsBitSchemaConformanceViolation::MissingTypedGrantSource;
			}

			if (entry->TypedGrantSource.rfind(L"CapabilityDescriptorSet.", 0) == 0)
			{
				return VmxCapsBitSchemaConformanceViolation::DescriptorSetClaimedAsTypedGrantSource;
			}

			if (entry->FrontendProjectionPolicy != CapabilityFrontendProjectionPolicy::ProjectIfCompatible)
			{
				return VmxCapsBitSchemaConformanceViolation::MissingProjectionPolicy;
			}

			if (entry->EvidenceVisibility == CapabilityEvidenceVisibility::HostOnly)
			{
				return VmxCapsBitSchemaConformanceViolation::HostOnlyEvidenceProjection;
			}
		}

		return VmxCapsBitSchemaConformanceViolation::None;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public bool IsSatisfied(CapabilityDescriptorSetSchema? schema, CapabilityBitSchemaArtifact artifact)
	bool VmxCapsBitSchemaConformanceContract::IsSatisfied(std::shared_ptr<CapabilityDescriptorSetSchema> schema, std::shared_ptr<CapabilityBitSchemaArtifact> artifact)
	{
		return Evaluate(schema, artifact) == VmxCapsBitSchemaConformanceViolation::None;
	}

	bool VmxCapsBitSchemaConformanceContract::IsCurrentSchemaSatisfied()
	{
		return IsSatisfied(CapabilityDescriptorSetSchema::getVmxCompatibility(), CapabilityDescriptorSetSchema::getVmxCompatibilityBitSchema());
	}
}
