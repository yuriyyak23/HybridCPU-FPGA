#include "SecureCompletionRetirePublicationAuthorityPolicy.h"
#include "../Policies/Backend/SecureBackendOwnerAdmissionPolicy.h"
#include "../Policies/Io/SecureIoHypercallAdmissionPolicy.h"
#include "../Policies/Backend/SecureHypercallBackendContractAdmissionPolicy.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<SecureCompletionRetirePublicationAuthorityPolicy> SecureCompletionRetirePublicationAuthorityPolicy::getDefault()
	{
		return Default;
	}

	std::shared_ptr<SecureCompletionRetirePublicationResult> SecureCompletionRetirePublicationAuthorityPolicy::AdmitCompletion(std::shared_ptr<SecureCompletionPublicationAuthorityRequest> request)
	{
		std::shared_ptr<SecureCompletionRetirePublicationResult> pathResult = DenyCurrentNonPublishingPath(request->PathKind);
		if (!pathResult->IsDenied || pathResult->Decision != SecureCompletionRetirePublicationDecision::DeniedNoBackendResult)
		{
			return pathResult;
		}

		if (request->BackendResultState != SecurePublicationBackendResultState::InternalNeutralResult)
		{
			return Deny(SecureCompletionRetirePublicationDecision::DeniedNoBackendResult, SecurePublicationLadderStep::NoBackendResult, L"Secure completion publication requires a separately produced neutral backend result.");
		}

		if (!IsCurrentNeutralOwner(request->BackendResultOwner, request->CurrentEpoch))
		{
			return Deny(SecureCompletionRetirePublicationDecision::DeniedBackendResultOwner, SecurePublicationLadderStep::InternalBackendResult, L"Secure completion publication requires a current neutral backend-result owner.");
		}

		if (!request->CompletionRecordMaterialized)
		{
			return Deny(SecureCompletionRetirePublicationDecision::DeniedCompletionRecordMissing, SecurePublicationLadderStep::InternalBackendResult, L"Secure completion publication requires an internal completion record after the backend result.");
		}

		std::optional<std::shared_ptr<SecureCompletionRetirePublicationResult>> owner = ValidateCompletionOwner(request->CompletionOwner, request->CurrentEpoch);
		if (owner)
		{
			return owner.value();
		}

		if (request->PublicationFence == nullptr || !request->PublicationFence->getCanPublishCompletion())
		{
			return Deny(SecureCompletionRetirePublicationDecision::DeniedCompletionFence, SecurePublicationLadderStep::InternalCompletionRecord, L"Secure completion publication requires a completion owner and completion fence.");
		}

		return SecureCompletionRetirePublicationResult::PublishedCompletion(L"Secure completion publication reached the completion step only; retire publication remains separate.");
	}

	std::shared_ptr<SecureCompletionRetirePublicationResult> SecureCompletionRetirePublicationAuthorityPolicy::AdmitRetire(std::shared_ptr<SecureRetirePublicationAuthorityRequest> request)
	{
		if (!request->CompletionPublication.CompletionPublished)
		{
			return Deny(SecureCompletionRetirePublicationDecision::DeniedCompletionPublicationRequired, SecurePublicationLadderStep::InternalCompletionRecord, L"Secure retire publication requires a previously published secure completion.");
		}

		std::optional<std::shared_ptr<SecureCompletionRetirePublicationResult>> owner = ValidateRetireOwner(request->RetireOwner, request->CurrentEpoch);
		if (owner)
		{
			return owner.value();
		}

		if (request->PublicationFence == nullptr || !request->PublicationFence->getCanPublishRetire())
		{
			return Deny(SecureCompletionRetirePublicationDecision::DeniedRetireFence, SecurePublicationLadderStep::CompletionPublication, L"Secure retire publication requires explicit retire owner authority and an explicit retire fence.");
		}

		if (!CanRetireEvidence(request->EvidenceClass))
		{
			return Deny(SecureCompletionRetirePublicationDecision::DeniedRetireEvidence, SecurePublicationLadderStep::CompletionPublication, L"Secure retire publication cannot expose host-owned runtime evidence.");
		}

		if (!CanRetireMigrationClass(request->MigrationClass))
		{
			return Deny(SecureCompletionRetirePublicationDecision::DeniedRetireMigrationClass, SecurePublicationLadderStep::CompletionPublication, L"Secure retire publication requires recomputed or guest-architectural migration classification.");
		}

		return SecureCompletionRetirePublicationResult::PublishedRetire();
	}

	std::shared_ptr<SecureCompletionRetirePublicationResult> SecureCompletionRetirePublicationAuthorityPolicy::AdmitCompletionFromBackendOwnerAdmission(std::shared_ptr<SecureBackendOwnerAdmissionResult> admission)
	{
		return admission->Decision == SecureBackendOwnerAdmissionDecision::AllowedProofOnlyNoExecution ? Deny(SecureCompletionRetirePublicationDecision::DeniedProofOnlyAdmission, SecurePublicationLadderStep::NoBackendResult, L"Proof-only backend owner admission cannot publish secure completion.") : DenyNoBackendResult();
	}

	std::shared_ptr<SecureCompletionRetirePublicationResult> SecureCompletionRetirePublicationAuthorityPolicy::AdmitRetireFromBackendOwnerAdmission(std::shared_ptr<SecureBackendOwnerAdmissionResult> admission)
	{
		return admission->Decision == SecureBackendOwnerAdmissionDecision::AllowedProofOnlyNoExecution ? Deny(SecureCompletionRetirePublicationDecision::DeniedProofOnlyAdmission, SecurePublicationLadderStep::NoBackendResult, L"Proof-only backend owner admission cannot publish secure retire effects.") : DenyNoBackendResult();
	}

	std::shared_ptr<SecureCompletionRetirePublicationResult> SecureCompletionRetirePublicationAuthorityPolicy::AdmitCompletionFromSecureIoHypercallAdmission(std::shared_ptr<SecureIoHypercallAdmissionResult> admission)
	{
		return admission->IsAdmittedDenied ? Deny(SecureCompletionRetirePublicationDecision::DeniedAdmittedDeniedAdmission, SecurePublicationLadderStep::NoBackendResult, L"Admitted-denied secure hypercall recognition cannot publish secure completion.") : admission->IsPolicyAdmissionOnly ? Deny(SecureCompletionRetirePublicationDecision::DeniedPolicyAdmissionOnly, SecurePublicationLadderStep::NoBackendResult, L"Secure I/O policy admission cannot publish secure completion.") : DenyNoBackendResult();
	}

	std::shared_ptr<SecureCompletionRetirePublicationResult> SecureCompletionRetirePublicationAuthorityPolicy::AdmitRetireFromSecureIoHypercallAdmission(std::shared_ptr<SecureIoHypercallAdmissionResult> admission)
	{
		return admission->IsAdmittedDenied ? Deny(SecureCompletionRetirePublicationDecision::DeniedAdmittedDeniedAdmission, SecurePublicationLadderStep::NoBackendResult, L"Admitted-denied secure hypercall recognition cannot publish secure retire effects.") : admission->IsPolicyAdmissionOnly ? Deny(SecureCompletionRetirePublicationDecision::DeniedPolicyAdmissionOnly, SecurePublicationLadderStep::NoBackendResult, L"Secure I/O policy admission cannot publish secure retire effects.") : DenyNoBackendResult();
	}

	std::shared_ptr<SecureCompletionRetirePublicationResult> SecureCompletionRetirePublicationAuthorityPolicy::AdmitCompletionFromHypercallContractAdmission(std::shared_ptr<SecureHypercallBackendContractAdmissionResult> admission)
	{
		return admission->IsProofOnly ? Deny(SecureCompletionRetirePublicationDecision::DeniedRegistryBackedProofOnlyAdmission, SecurePublicationLadderStep::NoBackendResult, L"Registry-backed Phase 13 owner/service admission remains proof-only and cannot publish secure completion.") : DenyNoBackendResult();
	}

	std::shared_ptr<SecureCompletionRetirePublicationResult> SecureCompletionRetirePublicationAuthorityPolicy::AdmitRetireFromHypercallContractAdmission(std::shared_ptr<SecureHypercallBackendContractAdmissionResult> admission)
	{
		return admission->IsProofOnly ? Deny(SecureCompletionRetirePublicationDecision::DeniedRegistryBackedProofOnlyAdmission, SecurePublicationLadderStep::NoBackendResult, L"Registry-backed Phase 13 owner/service admission remains proof-only and cannot publish secure retire effects.") : DenyNoBackendResult();
	}

	std::shared_ptr<SecureCompletionRetirePublicationResult> SecureCompletionRetirePublicationAuthorityPolicy::DenyGenericTrapRouteFlags(bool completionPublicationFlag, bool retirePublicationFlag)
	{
		return completionPublicationFlag || retirePublicationFlag ? Deny(SecureCompletionRetirePublicationDecision::DeniedGenericTrapRouteNotSecureAuthority, SecurePublicationLadderStep::NoBackendResult, L"Generic trap route publication flags are not SecureCompute authority without secure owner/path reachability.") : DenyNoBackendResult();
	}

	std::shared_ptr<SecureCompletionRetirePublicationResult> SecureCompletionRetirePublicationAuthorityPolicy::DenyVmxCompatibilityProjectionOnly()
	{
		return Deny(SecureCompletionRetirePublicationDecision::DeniedVmxProjectionOnly, SecurePublicationLadderStep::NoBackendResult, L"VMX compatibility projection is zero-authority for SecureCompute completion and retire publication.");
	}

	std::shared_ptr<SecureCompletionRetirePublicationResult> SecureCompletionRetirePublicationAuthorityPolicy::DenyCurrentNonPublishingPath(SecurePublicationPathKind pathKind)
	{
        switch (pathKind)
        {
        case SecurePublicationPathKind::ProofOnlyBackendOwnerAdmission:
            return Deny(SecureCompletionRetirePublicationDecision::DeniedProofOnlyAdmission, SecurePublicationLadderStep::NoBackendResult, L"Proof-only backend owner admission cannot publish secure completion.");
        case SecurePublicationPathKind::AdmittedDeniedSecureHypercallAdmission:
            return Deny(SecureCompletionRetirePublicationDecision::DeniedAdmittedDeniedAdmission, SecurePublicationLadderStep::NoBackendResult, L"Admitted-denied secure hypercall recognition cannot publish secure completion.");
        case SecurePublicationPathKind::RegistryBackedHypercallContractAdmission:
            return Deny(SecureCompletionRetirePublicationDecision::DeniedRegistryBackedProofOnlyAdmission, SecurePublicationLadderStep::NoBackendResult, L"Registry-backed Phase 13 owner/service admission remains proof-only and cannot publish secure completion.");
        case SecurePublicationPathKind::SecureIoPolicyAdmissionOnly:
            return Deny(SecureCompletionRetirePublicationDecision::DeniedPolicyAdmissionOnly, SecurePublicationLadderStep::NoBackendResult, L"Secure I/O policy admission cannot publish secure completion.");
        case SecurePublicationPathKind::GenericTrapRouteFlags:
            return Deny(SecureCompletionRetirePublicationDecision::DeniedGenericTrapRouteNotSecureAuthority, SecurePublicationLadderStep::NoBackendResult, L"Generic trap route publication flags are not SecureCompute authority without secure owner/path reachability.");
        case SecurePublicationPathKind::VmxCompatibilityProjectionOnly:
            return Deny(SecureCompletionRetirePublicationDecision::DeniedVmxProjectionOnly, SecurePublicationLadderStep::NoBackendResult, L"VMX compatibility projection is zero-authority for SecureCompute completion and retire publication.");
        default:
            return DenyNoBackendResult();
        }
	}

	std::optional<std::shared_ptr<SecureCompletionRetirePublicationResult>> SecureCompletionRetirePublicationAuthorityPolicy::ValidateCompletionOwner(std::optional<std::shared_ptr<SecureBackendOwnerDescriptor>> &owner, std::shared_ptr<SecureRevocationEpoch> currentEpoch)
	{
		std::optional<std::shared_ptr<SecureCompletionRetirePublicationResult>> common = ValidateOwner(owner, currentEpoch, SecureCompletionRetirePublicationDecision::DeniedCompletionOwnerMissing, SecureCompletionRetirePublicationDecision::DeniedCompletionOwnerSource, SecureCompletionRetirePublicationDecision::DeniedCompletionOwnerEpoch);
		if (common)
		{
			return common;
		}

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return owner!.Value.CompletionFenceValidated ? null : Deny( SecureCompletionRetirePublicationDecision.DeniedCompletionOwnerProof, SecurePublicationLadderStep.InternalCompletionRecord, "Secure completion owner requires validated completion-fence proof.");
		if (owner.value()->CompletionFenceValidated)
		{
			return std::nullopt;
		}
		return Deny(SecureCompletionRetirePublicationDecision::DeniedCompletionOwnerProof, SecurePublicationLadderStep::InternalCompletionRecord, L"Secure completion owner requires validated completion-fence proof.");
	}

	std::optional<std::shared_ptr<SecureCompletionRetirePublicationResult>> SecureCompletionRetirePublicationAuthorityPolicy::ValidateRetireOwner(std::optional<std::shared_ptr<SecureBackendOwnerDescriptor>> &owner, std::shared_ptr<SecureRevocationEpoch> currentEpoch)
	{
		std::optional<std::shared_ptr<SecureCompletionRetirePublicationResult>> common = ValidateOwner(owner, currentEpoch, SecureCompletionRetirePublicationDecision::DeniedRetireOwnerMissing, SecureCompletionRetirePublicationDecision::DeniedRetireOwnerSource, SecureCompletionRetirePublicationDecision::DeniedRetireOwnerEpoch);
		if (common)
		{
			return common;
		}

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return owner!.Value.RetireFenceValidated ? null : Deny( SecureCompletionRetirePublicationDecision.DeniedRetireOwnerProof, SecurePublicationLadderStep.CompletionPublication, "Secure retire owner requires validated retire-fence proof.");
		if (owner.value()->RetireFenceValidated)
		{
			return std::nullopt;
		}
		return Deny(SecureCompletionRetirePublicationDecision::DeniedRetireOwnerProof, SecurePublicationLadderStep::CompletionPublication, L"Secure retire owner requires validated retire-fence proof.");
	}

	std::optional<std::shared_ptr<SecureCompletionRetirePublicationResult>> SecureCompletionRetirePublicationAuthorityPolicy::ValidateOwner(std::optional<std::shared_ptr<SecureBackendOwnerDescriptor>> &owner, std::shared_ptr<SecureRevocationEpoch> currentEpoch, SecureCompletionRetirePublicationDecision missingDecision, SecureCompletionRetirePublicationDecision sourceDecision, SecureCompletionRetirePublicationDecision epochDecision)
	{
		if (!owner || !owner.value() || !owner.value()->Materialized || !owner.value()->HasIdentity)
		{
			return Deny(missingDecision, SecurePublicationLadderStep::InternalCompletionRecord, L"Secure publication owner requires a materialized owner identity.");
		}

		if (!owner.value()->IsNeutralSource)
		{
			return Deny(sourceDecision, SecurePublicationLadderStep::InternalCompletionRecord, L"Secure publication owner cannot be sourced from VMX, VMCS, VmxCaps or compatibility projection.");
		}

		if (!owner.value()->Epoch->Equals(currentEpoch))
		{
			return Deny(epochDecision, SecurePublicationLadderStep::InternalCompletionRecord, L"Secure publication owner epoch must match the current policy epoch.");
		}

		return std::nullopt;
	}

	bool SecureCompletionRetirePublicationAuthorityPolicy::IsCurrentNeutralOwner(std::optional<std::shared_ptr<SecureBackendOwnerDescriptor>> &owner, std::shared_ptr<SecureRevocationEpoch> currentEpoch)
	{
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		return owner && owner.value() && owner.value()->Materialized && owner.value()->HasIdentity && owner.value()->IsNeutralSource && owner.value()->Epoch && owner.value()->Epoch->Equals(currentEpoch);
	}

	bool SecureCompletionRetirePublicationAuthorityPolicy::CanRetireEvidence(EvidenceVisibilityClass evidenceClass)
	{
		return (evidenceClass == EvidenceVisibilityClass::GuestArchitecturalState || evidenceClass == EvidenceVisibilityClass::CompatibilityAlias);
	}

	bool SecureCompletionRetirePublicationAuthorityPolicy::CanRetireMigrationClass(TrapCompletionMigrationClass migrationClass)
	{
		return (migrationClass == TrapCompletionMigrationClass::RecomputedAfterRestore || migrationClass == TrapCompletionMigrationClass::GuestArchitecturalState);
	}

	std::shared_ptr<SecureCompletionRetirePublicationResult> SecureCompletionRetirePublicationAuthorityPolicy::DenyNoBackendResult()
	{
		return Deny(SecureCompletionRetirePublicationDecision::DeniedNoBackendResult, SecurePublicationLadderStep::NoBackendResult, L"Secure completion/retire publication requires a neutral backend result and explicit publication owners.");
	}

	std::shared_ptr<SecureCompletionRetirePublicationResult> SecureCompletionRetirePublicationAuthorityPolicy::Deny(SecureCompletionRetirePublicationDecision decision, SecurePublicationLadderStep ladderStep, const std::wstring &reason)
	{
		return SecureCompletionRetirePublicationResult::Denied(decision, ladderStep, reason);
	}
}
