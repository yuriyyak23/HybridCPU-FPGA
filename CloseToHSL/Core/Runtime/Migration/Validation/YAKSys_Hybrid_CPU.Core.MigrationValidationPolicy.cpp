#include "YAKSys_Hybrid_CPU.Core.MigrationValidationPolicy.h"
#include "MigrationValidationPolicy.h"

namespace YAKSys_Hybrid_CPU::Core
{

		const std::shared_ptr<MigrationValidationResult> MigrationValidationResult::Allowed =
		std::make_shared<MigrationValidationResult>(MigrationValidationDecision::Allowed, std::wstring{});

MigrationValidationPolicy::MigrationValidationPolicy() : MigrationValidationPolicy(MigrationDescriptor::getFailClosed(), EvidencePolicyDescriptor::getFailClosed(), true, true)
	{
	}

	MigrationValidationPolicy::MigrationValidationPolicy(std::shared_ptr<MigrationDescriptor> migration, std::shared_ptr<EvidencePolicyDescriptor> evidence, bool rejectCompatibilityProjectionMetadata, bool requireGuestStatePreservePolicy)
		: Migration(std::move(migration)),
		  Evidence(std::move(evidence)),
		  RejectCompatibilityProjectionMetadata(rejectCompatibilityProjectionMetadata),
		  RequireGuestStatePreservePolicy(requireGuestStatePreservePolicy)
	{
	}

	std::shared_ptr<MigrationValidationPolicy> MigrationValidationPolicy::getFailClosed()
	{
		return FailClosed;
	}

	std::shared_ptr<MigrationDescriptor> MigrationValidationPolicy::getMigration() const
	{
		return Migration;
	}

	std::shared_ptr<EvidencePolicyDescriptor> MigrationValidationPolicy::getEvidence() const
	{
		return Evidence;
	}

	bool MigrationValidationPolicy::getRejectCompatibilityProjectionMetadata() const
	{
		return RejectCompatibilityProjectionMetadata;
	}

	bool MigrationValidationPolicy::getRequireGuestStatePreservePolicy() const
	{
		return RequireGuestStatePreservePolicy;
	}

	std::shared_ptr<MigrationValidationResult> MigrationValidationPolicy::ValidateImport(MigrationPayloadClass payloadClass, EvidenceVisibilityClass evidenceClass, EvidenceRestorePolicy restorePolicy)
	{
		if (getMigration()->MustRejectImportedPayload(payloadClass))
		{
			return MigrationValidationResult::Denied(MigrationValidationDecision::PayloadClassDenied, L"Migration descriptor rejects this payload class.");
		}

		if (getRejectCompatibilityProjectionMetadata() && payloadClass == MigrationPayloadClass::CompatibilityProjectionMetadata)
		{
			return MigrationValidationResult::Denied(MigrationValidationDecision::PayloadClassDenied, L"Compatibility projection metadata is not authoritative restore state.");
		}

		if (getMigration()->MustRecomputeAfterRestore(payloadClass) || getEvidence()->MustRecomputeAfterRestore(evidenceClass))
		{
			return MigrationValidationResult::Denied(MigrationValidationDecision::HostOwnedEvidenceRejected, L"Host-owned runtime evidence must be recomputed after restore.");
		}

		if (getRequireGuestStatePreservePolicy() && evidenceClass == EvidenceVisibilityClass::GuestArchitecturalState && restorePolicy != EvidenceRestorePolicy::PreserveGuestArchitecturalState)
		{
			return MigrationValidationResult::Denied(MigrationValidationDecision::RestorePolicyRejected, L"Guest architectural state import requires preserve restore policy.");
		}

		if (!getEvidence()->CanSerializeAcrossMigration(evidenceClass, restorePolicy))
		{
			return MigrationValidationResult::Denied(MigrationValidationDecision::EvidencePolicyDenied, L"Evidence policy denies migration serialization for this evidence class.");
		}

		return MigrationValidationResult::Allowed;
	}
}


