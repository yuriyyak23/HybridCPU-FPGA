#include "YAKSys_Hybrid_CPU.Core.SecureNestedDomainAdmissionPolicy.h"
#include "../../Descriptors/Nested/YAKSys_Hybrid_CPU.Core.SecureChildDomainIntentDescriptor.h"
#include "../../Authority/SecurePolicyDerivationRecord.h"
#include "../../Descriptors/Domain/YAKSys_Hybrid_CPU.Core.SecureComputeDomainDescriptor.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<SecureNestedDomainAdmissionPolicy> SecureNestedDomainAdmissionPolicy::getDefault()
	{
		return Default;
	}

	std::shared_ptr<SecureNestedDomainAdmissionResult> SecureNestedDomainAdmissionPolicy::Admit(std::shared_ptr<SecureNestedDomainAdmissionRequest> request)
	{
		if (request == nullptr)
		{
			return Deny(SecureNestedDomainAdmissionDecision::DeniedMissingParentSecureDescriptor, L"Nested secure admission requires an explicit admission request.");
		}

		std::shared_ptr<SecureNestedDomainAdmissionResult> checkpoint = AdmitCheckpointPayload(request->CheckpointPayloadClass, request->ShadowVmcsStoresMutableAuthority);
		if (!checkpoint->IsAllowed)
		{
			return checkpoint;
		}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: SecureChildDomainIntentDescriptor? childIntent = request.ChildIntent;
		std::shared_ptr<SecureChildDomainIntentDescriptor> childIntent = request->ChildIntent;
		if (childIntent == nullptr || !childIntent->isSecureRequest())
		{
			return SecureNestedDomainAdmissionResult::AllowedNoEffect;
		}

		if (!request->HasNeutralChildIntentOwner)
		{
			return Deny(SecureNestedDomainAdmissionDecision::DeniedMissingChildIntentOwner, L"Nested secure child admission requires a neutral runtime-owned child intent descriptor.");
		}

		if (request->ParentDescriptor == nullptr || !request->ParentDescriptor->isActive())
		{
			return Deny(SecureNestedDomainAdmissionDecision::DeniedMissingParentSecureDescriptor, L"Nested secure child admission requires a materialized enabled parent secure descriptor.");
		}

		if (!childIntent->isMaterialized())
		{
			return Deny(SecureNestedDomainAdmissionDecision::DeniedUnmaterializedChildIntent, L"Nested secure child intent must name materialized parent and child domain tags.");
		}

		if (request->ParentHostEvidenceExposedToChild || request->ChildHostEvidenceExposedToParent)
		{
			return Deny(SecureNestedDomainAdmissionDecision::DeniedHostEvidenceLeakage, L"Host-owned nested secure evidence must remain quarantined from child and parent guest-visible state.");
		}

		if (request->NestedProjectionExceedsParent)
		{
			return Deny(SecureNestedDomainAdmissionDecision::DeniedNestedProjectionExpansion, L"Nested compatibility projection cannot expose more than the parent policy allows.");
		}

		SecurePolicyDerivationDecision derivation = childIntent->ValidateMonotonicDerivation((request->ParentBounds != nullptr) ? request->ParentBounds : SecureAuthorityBounds::getNone(), request->CurrentEpoch);

		switch (derivation)
		{
		case SecurePolicyDerivationDecision::AllowedSubset:
			return SecureNestedDomainAdmissionResult::AllowedDesignFence;
		case SecurePolicyDerivationDecision::ProvenanceMissing:
			return Deny(SecureNestedDomainAdmissionDecision::DeniedChildIntentProvenanceMissing, L"Nested secure child policy derivation requires runtime provenance.");
		case SecurePolicyDerivationDecision::EpochMismatch:
			return Deny(SecureNestedDomainAdmissionDecision::DeniedStaleParentChildEpoch, L"Nested secure parent and child policy derivation must match the current epoch.");
		case SecurePolicyDerivationDecision::ChildExpandsAuthority:
			return DenyChildExpansion((request->ParentBounds != nullptr) ? request->ParentBounds : SecureAuthorityBounds::getNone(), childIntent->getRequestedBounds());
		default:
			return Deny(SecureNestedDomainAdmissionDecision::DeniedMissingParentSecureDescriptor, L"Nested secure child derivation requires a materialized parent authority boundary.");
		}
	}

	std::shared_ptr<SecureNestedDomainAdmissionResult> SecureNestedDomainAdmissionPolicy::AdmitCheckpointPayload(SecureNestedCheckpointPayloadClass payloadClass, bool shadowVmcsStoresMutableAuthority)
	{
		if ((payloadClass == SecureNestedCheckpointPayloadClass::Vmcs12Authority || payloadClass == SecureNestedCheckpointPayloadClass::Vmcs02Authority))
		{
			return Deny(SecureNestedDomainAdmissionDecision::DeniedNestedVmcsAuthority, L"Nested secure checkpoint authority cannot be sourced from VMCS12 or VMCS02 state.");
		}

		if (payloadClass == SecureNestedCheckpointPayloadClass::MutableShadowVmcsAuthority || shadowVmcsStoresMutableAuthority)
		{
			return Deny(SecureNestedDomainAdmissionDecision::DeniedMutableShadowVmcsAuthority, L"Shadow VMCS remains a compatibility bridge and cannot own mutable nested secure authority.");
		}

		return SecureNestedDomainAdmissionResult::AllowedDesignFence;
	}

	std::shared_ptr<SecureNestedDomainAdmissionResult> SecureNestedDomainAdmissionPolicy::DenyChildExpansion(std::shared_ptr<SecureAuthorityBounds> parentBounds, std::shared_ptr<SecureAuthorityBounds> childBounds)
	{
		if (parentBounds == nullptr || childBounds == nullptr)
		{
			return Deny(SecureNestedDomainAdmissionDecision::DeniedChildPolicyExpansion, L"Nested child authority bounds must be materialized before expansion checks.");
		}

		if (childBounds->getAllowsCompatibilityProjection() && !parentBounds->getAllowsCompatibilityProjection())
		{
			return Deny(SecureNestedDomainAdmissionDecision::DeniedChildCompatibilityProjectionExpansion, L"Nested child compatibility projection policy cannot exceed the parent policy.");
		}

		if (childBounds->getAllowsMigration() && !parentBounds->getAllowsMigration())
		{
			return Deny(SecureNestedDomainAdmissionDecision::DeniedChildMigrationPayloadExpansion, L"Nested child migration payload policy cannot exceed the parent policy.");
		}

		return Deny(SecureNestedDomainAdmissionDecision::DeniedChildPolicyExpansion, L"Nested child secure policy cannot widen parent memory, I/O, hypercall or debug authority.");
	}

	std::shared_ptr<SecureNestedDomainAdmissionResult> SecureNestedDomainAdmissionPolicy::Deny(SecureNestedDomainAdmissionDecision decision, const std::wstring &reason)
	{
		return SecureNestedDomainAdmissionResult::Denied(decision, reason);
	}
}
