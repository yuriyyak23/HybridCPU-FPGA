#include "YAKSys_Hybrid_CPU.Core.VmcsFieldProjectionSchemaConformanceContract.h"
#include <array>
#include <algorithm>
#include <unordered_set>
#include "../../Compatibility/Generated/VmcsProjection/VmcsFieldProjectionSchema.h"
#include "../../../Runtime/Domains/Descriptors/EvidencePolicy/EvidencePolicyDescriptor.h"

namespace YAKSys_Hybrid_CPU::Core
{

	VmcsFieldProjectionSchemaConformanceViolation VmcsFieldProjectionSchemaConformanceContract::Evaluate(std::shared_ptr<VmcsFieldProjectionSchemaArtifact> artifact)
	{
		if (!artifact->IsCanonical || artifact->SchemaVersion != VmcsFieldProjectionSchema::CurrentVersion || artifact->SourceHash == 0 || artifact->ArtifactHash == 0)
		{
			return VmcsFieldProjectionSchemaConformanceViolation::MissingCanonicalArtifact;
		}

		std::unordered_set<VmcsField> seen;
		for (auto entry : VmcsFieldProjectionSchema::getEntries())
		{
			if (!seen.insert(entry->Field).second)
			{
				return VmcsFieldProjectionSchemaConformanceViolation::DuplicateField;
			}

			if (!entry->IsGeneratedAlias)
			{
				return VmcsFieldProjectionSchemaConformanceViolation::MissingGeneratedAlias;
			}

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'typeof' operator:
			if (static_cast<unsigned char>(entry->Owner) > static_cast<unsigned char>(VmcsFieldProjectionOwner::CompatibilityControlDescriptor))
			{
				return VmcsFieldProjectionSchemaConformanceViolation::MissingOwner;
			}

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'typeof' operator:
			if (static_cast<unsigned char>(entry->AccessPolicy) > static_cast<unsigned char>(VmcsFieldProjectionAccessPolicy::ReadWrite) || entry->AccessPolicy == VmcsFieldProjectionAccessPolicy::Denied)
			{
				return VmcsFieldProjectionSchemaConformanceViolation::MissingAccessPolicy;
			}

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'typeof' operator:
			if (static_cast<unsigned char>(entry->MigrationPolicy) > static_cast<unsigned char>(VmcsFieldProjectionMigrationPolicy::ProjectionOnly) || entry->MigrationPolicy == VmcsFieldProjectionMigrationPolicy::None)
			{
				return VmcsFieldProjectionSchemaConformanceViolation::MissingMigrationPolicy;
			}

			if (entry->Owner == VmcsFieldProjectionOwner::CompletionRecord && VmcsFieldProjectionSchema::CanWrite(entry))
			{
				return VmcsFieldProjectionSchemaConformanceViolation::CompletionFieldWritable;
			}

			if (IsHostCompatibilityAlias(entry->Field) && VmcsFieldProjectionSchema::CanWrite(entry))
			{
				return VmcsFieldProjectionSchemaConformanceViolation::HostAliasWritable;
			}

			if (entry->EvidenceClass == EvidenceVisibilityClass::HostOwnedRuntimeEvidence)
			{
				return VmcsFieldProjectionSchemaConformanceViolation::HostEvidenceExposed;
			}

			if (VmcsFieldProjectionSchema::CanWrite(entry))
			{
				return VmcsFieldProjectionSchemaConformanceViolation::FieldAliasWritable;
			}

			if (entry->EvidenceClass == EvidenceVisibilityClass::GuestArchitecturalState && entry->MigrationPolicy == VmcsFieldProjectionMigrationPolicy::ProjectionOnly)
			{
				return VmcsFieldProjectionSchemaConformanceViolation::ProjectionOnlyGuestArchitecturalState;
			}
		}

		static constexpr std::array<VmcsField, 23> frozenFields = { VmcsField::GuestPc, VmcsField::GuestSp, VmcsField::GuestFlags, VmcsField::GuestCr0, VmcsField::GuestCr3, VmcsField::GuestCr4, VmcsField::HostPc, VmcsField::HostSp, VmcsField::HostFlags, VmcsField::HostCr0, VmcsField::HostCr3, VmcsField::PinBasedControls, VmcsField::ProcBasedControls, VmcsField::ExitControls, VmcsField::EntryControls, VmcsField::EptPointer, VmcsField::Vpid, VmcsField::SecondaryProcControls, VmcsField::Cr3TargetCount, VmcsField::ExitReason, VmcsField::ExitQualification, VmcsField::GuestPhysicalAddress, VmcsField::EptViolationQualification };
		for (VmcsField field : frozenFields)
		{
			if (std::find(seen.begin(), seen.end(), field) == seen.end())
			{
				return VmcsFieldProjectionSchemaConformanceViolation::MissingFrozenField;
			}
		}

		return artifact->EntryCount == seen.size() ? VmcsFieldProjectionSchemaConformanceViolation::None : VmcsFieldProjectionSchemaConformanceViolation::EntryCountMismatch;
	}

	bool VmcsFieldProjectionSchemaConformanceContract::IsSatisfied(std::shared_ptr<VmcsFieldProjectionSchemaArtifact> artifact)
	{
		return Evaluate(artifact) == VmcsFieldProjectionSchemaConformanceViolation::None;
	}

	bool VmcsFieldProjectionSchemaConformanceContract::IsCurrentSchemaSatisfied()
	{
		return IsSatisfied(VmcsFieldProjectionSchema::getCanonicalArtifact());
	}

	bool VmcsFieldProjectionSchemaConformanceContract::IsHostCompatibilityAlias(VmcsField field)
	{
		return field == VmcsField::HostPc || field == VmcsField::HostSp || field == VmcsField::HostFlags || field == VmcsField::HostCr0 || field == VmcsField::HostCr3;
	}
}
