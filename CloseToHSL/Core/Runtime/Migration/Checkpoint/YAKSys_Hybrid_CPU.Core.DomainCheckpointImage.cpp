#include "YAKSys_Hybrid_CPU.Core.DomainCheckpointImage.h"
#include "../Validation/MigrationValidationPolicy.h"

namespace YAKSys_Hybrid_CPU::Core
{

	DomainCheckpointImage::DomainCheckpointImage() : DomainCheckpointImage(DomainCheckpointAuthority::DomainDescriptor, 0, 0, 0, false)
	{
	}

	DomainCheckpointImage::DomainCheckpointImage(DomainCheckpointAuthority authority, unsigned long long checkpointEpoch, unsigned long long payloadMask, unsigned long long evidenceMask, bool containsCompatibilityProjectionMetadata)
		: Authority(authority), CheckpointEpoch(checkpointEpoch), PayloadMask(payloadMask), EvidenceMask(evidenceMask), ContainsCompatibilityProjectionMetadata(containsCompatibilityProjectionMetadata)
	{
	}

	std::shared_ptr<DomainCheckpointImage> DomainCheckpointImage::getEmpty()
	{
		return Empty;
	}

	DomainCheckpointAuthority DomainCheckpointImage::getAuthority() const
	{
		return Authority;
	}

	unsigned long long DomainCheckpointImage::getCheckpointEpoch() const
	{
		return CheckpointEpoch;
	}

	unsigned long long DomainCheckpointImage::getPayloadMask() const
	{
		return PayloadMask;
	}

	unsigned long long DomainCheckpointImage::getEvidenceMask() const
	{
		return EvidenceMask;
	}

	bool DomainCheckpointImage::getContainsCompatibilityProjectionMetadata() const
	{
		return ContainsCompatibilityProjectionMetadata;
	}

	bool DomainCheckpointImage::isDomainAuthoritative() const
	{
		return getAuthority() == DomainCheckpointAuthority::DomainDescriptor;
	}

	bool DomainCheckpointImage::isEmpty() const
	{
		return getPayloadMask() == 0 && getEvidenceMask() == 0;
	}

	bool DomainCheckpointImage::getContainsHostOwnedEvidence() const
	{
		return IncludesEvidence(EvidenceVisibilityClass::HostOwnedRuntimeEvidence) || IncludesEvidence(EvidenceVisibilityClass::SchedulerEvidence) || IncludesEvidence(EvidenceVisibilityClass::BackendBindingEvidence) || IncludesEvidence(EvidenceVisibilityClass::NativeTokenEvidence);
	}

	bool DomainCheckpointImage::IncludesPayload(MigrationPayloadClass payloadClass) const
	{
		return (getPayloadMask() & ToMask(payloadClass)) != 0;
	}

	bool DomainCheckpointImage::IncludesEvidence(EvidenceVisibilityClass evidenceClass) const
	{
		return (getEvidenceMask() & ToMask(evidenceClass)) != 0;
	}

	std::shared_ptr<MigrationValidationResult> DomainCheckpointImage::ValidateRestore(std::shared_ptr<MigrationValidationPolicy> policy, EvidenceRestorePolicy restorePolicy)
	{
		if (!policy)
		{
			return MigrationValidationResult::Denied(MigrationValidationDecision::RestorePolicyRejected,
				L"Checkpoint restore requires a migration validation policy.");
		}

		if (!isDomainAuthoritative())
		{
			return MigrationValidationResult::Denied(MigrationValidationDecision::PayloadClassDenied, L"Compatibility projection checkpoint cannot restore authoritative domain state.");
		}

		if (getContainsHostOwnedEvidence())
		{
			return MigrationValidationResult::Denied(MigrationValidationDecision::HostOwnedEvidenceRejected, L"Domain checkpoint image contains host-owned evidence.");
		}

		for (auto payloadClass : getPayloadClasses())
		{
			if (!IncludesPayload(payloadClass))
			{
				continue;
			}

			EvidenceVisibilityClass evidenceClass = ToEvidenceClass(payloadClass);
			std::shared_ptr<MigrationValidationResult> result = policy->ValidateImport(payloadClass, evidenceClass, restorePolicy);
			if (!result || !result->IsAllowed)
			{
				return result;
			}
		}

		return MigrationValidationResult::Allowed;
	}

	std::shared_ptr<DomainCheckpointImage> DomainCheckpointImage::WithPayload(MigrationPayloadClass payloadClass)
	{
		return std::make_shared<DomainCheckpointImage>(getAuthority(), getCheckpointEpoch(), getPayloadMask() | ToMask(payloadClass), getEvidenceMask() | ToMask(ToEvidenceClass(payloadClass)), getContainsCompatibilityProjectionMetadata() || payloadClass == MigrationPayloadClass::CompatibilityProjectionMetadata);
	}

	unsigned long long DomainCheckpointImage::ToMask(MigrationPayloadClass payloadClass)
	{
		return 1ULL << static_cast<unsigned char>(payloadClass);
	}

	unsigned long long DomainCheckpointImage::ToMask(EvidenceVisibilityClass evidenceClass)
	{
		return 1ULL << static_cast<unsigned char>(evidenceClass);
	}

	EvidenceVisibilityClass DomainCheckpointImage::ToEvidenceClass(MigrationPayloadClass payloadClass)
	{
		switch (payloadClass)
		{
		case MigrationPayloadClass::GuestArchitecturalState: return EvidenceVisibilityClass::GuestArchitecturalState;
		case MigrationPayloadClass::CompatibilityProjectionMetadata: return EvidenceVisibilityClass::CompatibilityAlias;
		case MigrationPayloadClass::HostOwnedRuntimeEvidence: return EvidenceVisibilityClass::HostOwnedRuntimeEvidence;
		case MigrationPayloadClass::SchedulerEvidence: return EvidenceVisibilityClass::SchedulerEvidence;
		case MigrationPayloadClass::BackendBindingEvidence: return EvidenceVisibilityClass::BackendBindingEvidence;
		case MigrationPayloadClass::NativeTokenEvidence: return EvidenceVisibilityClass::NativeTokenEvidence;
		default: return EvidenceVisibilityClass::CompatibilityAlias;
		}
	}

	std::vector<MigrationPayloadClass> DomainCheckpointImage::getPayloadClasses()
	{
		return PayloadClasses;
	}
}


