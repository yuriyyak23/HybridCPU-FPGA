#include "YAKSys_Hybrid_CPU.Core.SecureEvidencePublicationPolicy.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<SecureEvidencePublicationPolicy> SecureEvidencePublicationPolicy::getDefault()
	{
		return Default;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SecureEvidencePublicationResult AdmitGuestVisibleEvidence(SecureEvidencePolicy? secureEvidencePolicy, System.Nullable<EvidencePolicyDescriptor> neutralEvidencePolicy, SecureEvidenceVisibilityClass evidenceClass)
	std::shared_ptr<SecureEvidencePublicationResult> SecureEvidencePublicationPolicy::AdmitGuestVisibleEvidence(std::shared_ptr<SecureEvidencePolicy> secureEvidencePolicy, std::shared_ptr<EvidencePolicyDescriptor> neutralEvidencePolicy, SecureEvidenceVisibilityClass evidenceClass)
	{
		std::shared_ptr<SecureEvidencePublicationResult> prerequisite = ValidateEvidencePolicyPrerequisites(secureEvidencePolicy, neutralEvidencePolicy, evidenceClass);
		if (!prerequisite->IsAllowed)
		{
			return prerequisite;
		}

		EvidenceVisibilityClass neutralClass = evidenceClass == SecureEvidenceVisibilityClass::CompatibilityAlias ? EvidenceVisibilityClass::CompatibilityAlias : EvidenceVisibilityClass::GuestArchitecturalState;

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: if (!neutralEvidencePolicy!.CanExposeToGuest(neutralClass))
		if (!neutralEvidencePolicy->CanExposeToGuest(neutralClass))
		{
			return Deny(SecureEvidencePublicationDecision::DeniedNeutralEvidencePolicy, L"Neutral evidence policy does not allow this evidence visibility class.");
		}

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: if (!secureEvidencePolicy!.CanExposeToGuest(neutralEvidencePolicy, evidenceClass))
		if (!secureEvidencePolicy->CanExposeToGuest(neutralEvidencePolicy, evidenceClass))
		{
			return Deny(SecureEvidencePublicationDecision::DeniedSecureEvidencePolicy, L"Secure evidence policy does not allow this evidence visibility class.");
		}

		return SecureEvidencePublicationResult::Allowed;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SecureEvidencePublicationResult AdmitCompatibilityAliasEvidence(SecureEvidencePolicy? secureEvidencePolicy, System.Nullable<EvidencePolicyDescriptor> neutralEvidencePolicy, System.Nullable<SecureCompatibilityProjectionPolicy> compatibilityPolicy, ulong aliasBit)
	std::shared_ptr<SecureEvidencePublicationResult> SecureEvidencePublicationPolicy::AdmitCompatibilityAliasEvidence(std::shared_ptr<SecureEvidencePolicy> secureEvidencePolicy, std::shared_ptr<EvidencePolicyDescriptor> neutralEvidencePolicy, std::shared_ptr<SecureCompatibilityProjectionPolicy> compatibilityPolicy, unsigned long long aliasBit)
	{
		std::shared_ptr<SecureEvidencePublicationResult> visibility = AdmitGuestVisibleEvidence(secureEvidencePolicy, neutralEvidencePolicy, SecureEvidenceVisibilityClass::CompatibilityAlias);
		if (!visibility->IsAllowed)
		{
			return visibility;
		}

		if (compatibilityPolicy == nullptr || !compatibilityPolicy->CanProjectAlias(aliasBit))
		{
			return Deny(SecureEvidencePublicationDecision::DeniedCompatibilityAliasPolicy, L"Compatibility alias evidence requires an explicit read-only compatibility projection policy.");
		}

		return SecureEvidencePublicationResult::Allowed;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SecureEvidencePublicationResult AdmitCompletionPublication(SecureCompletionPublicationFence? publicationFence)
	std::shared_ptr<SecureEvidencePublicationResult> SecureEvidencePublicationPolicy::AdmitCompletionPublication(std::shared_ptr<SecureCompletionPublicationFence> publicationFence)
	{
		if (publicationFence == nullptr || !publicationFence->getCanPublishCompletion())
		{
			return Deny(SecureEvidencePublicationDecision::DeniedCompletionFence, L"Secure completion publication requires an explicit completion fence.");
		}

		return SecureEvidencePublicationResult::Allowed;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SecureEvidencePublicationResult AdmitRetirePublication(SecureCompletionPublicationFence? publicationFence)
	std::shared_ptr<SecureEvidencePublicationResult> SecureEvidencePublicationPolicy::AdmitRetirePublication(std::shared_ptr<SecureCompletionPublicationFence> publicationFence)
	{
		if (publicationFence == nullptr || !publicationFence->getCanPublishRetire())
		{
			return Deny(SecureEvidencePublicationDecision::DeniedRetireFence, L"Secure retire publication requires an explicit retire fence.");
		}

		return SecureEvidencePublicationResult::Allowed;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SecureEvidencePublicationResult AdmitSidebandEnvelope(SecureComputeEvidenceSidebandEnvelope envelope, SecureEvidencePolicy? secureEvidencePolicy, System.Nullable<EvidencePolicyDescriptor> neutralEvidencePolicy, System.Nullable<SecureCompatibilityProjectionPolicy> compatibilityPolicy = null, ulong aliasBit = 0)
	std::shared_ptr<SecureEvidencePublicationResult> SecureEvidencePublicationPolicy::AdmitSidebandEnvelope(std::shared_ptr<SecureComputeEvidenceSidebandEnvelope> envelope, std::shared_ptr<SecureEvidencePolicy> secureEvidencePolicy, std::shared_ptr<EvidencePolicyDescriptor> neutralEvidencePolicy, std::shared_ptr<SecureCompatibilityProjectionPolicy> compatibilityPolicy, unsigned long long aliasBit)
	{
		if (envelope == nullptr)
		{
			return Deny(SecureEvidencePublicationDecision::DeniedSidebandVisibility, L"Secure evidence publication requires a sideband envelope.");
		}

		if (envelope->getEvidenceClass() == SecureComputeEvidenceSidebandClass::CompatibilityAlias)
		{
			std::shared_ptr<SecureEvidencePublicationResult> aliasResult = AdmitCompatibilityAliasEvidence(secureEvidencePolicy, neutralEvidencePolicy, compatibilityPolicy, aliasBit);
			return aliasResult->IsAllowed ? aliasResult : Deny(SecureEvidencePublicationDecision::DeniedSidebandVisibility, aliasResult->Reason);
		}

		SecureEvidenceVisibilityClass evidenceClass;
		switch (envelope->getEvidenceClass())
		{
		case SecureComputeEvidenceSidebandClass::GuestVisible:
			evidenceClass = SecureEvidenceVisibilityClass::GuestVisible;
			break;
		case SecureComputeEvidenceSidebandClass::RecomputeAfterRestore:
			evidenceClass = SecureEvidenceVisibilityClass::RecomputedAfterRestore;
			break;
		default:
			evidenceClass = SecureEvidenceVisibilityClass::HostOwnedQuarantined;
			break;
		}

		std::shared_ptr<SecureEvidencePublicationResult> result = AdmitGuestVisibleEvidence(secureEvidencePolicy, neutralEvidencePolicy, evidenceClass);
		if (!result->IsAllowed)
		{
			return Deny(result->Decision == SecureEvidencePublicationDecision::DeniedHostOwnedEvidence || result->Decision == SecureEvidencePublicationDecision::DeniedRecomputedEvidence ? result->Decision : SecureEvidencePublicationDecision::DeniedSidebandVisibility, result->Reason);
		}

		return SecureEvidencePublicationResult::Allowed;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SecureEvidencePublicationResult AdmitReplayOrRestoreEvidence(DomainMeasurementDescriptor? measurement, SecureRevocationEpoch expectedEpoch)
	std::shared_ptr<SecureEvidencePublicationResult> SecureEvidencePublicationPolicy::AdmitReplayOrRestoreEvidence(std::shared_ptr<DomainMeasurementDescriptor> measurement, std::shared_ptr<SecureRevocationEpoch> expectedEpoch)
	{
		if (measurement == nullptr || ([&] { const auto codexPatternValue = measurement->getState(); return codexPatternValue == SecureMeasurementState::Missing || codexPatternValue == SecureMeasurementState::Pending || codexPatternValue == SecureMeasurementState::Stale || codexPatternValue == SecureMeasurementState::Revoked; }()) || !measurement->isMaterialized() || !measurement->IsCurrentFor(expectedEpoch))
		{
			return Deny(SecureEvidencePublicationDecision::DeniedStaleEvidenceEpoch, L"Secure evidence replay or restore requires a materialized measurement at the current epoch.");
		}

		return SecureEvidencePublicationResult::Allowed;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static SecureEvidencePublicationResult ValidateEvidencePolicyPrerequisites(SecureEvidencePolicy? secureEvidencePolicy, System.Nullable<EvidencePolicyDescriptor> neutralEvidencePolicy, SecureEvidenceVisibilityClass evidenceClass)
	std::shared_ptr<SecureEvidencePublicationResult> SecureEvidencePublicationPolicy::ValidateEvidencePolicyPrerequisites(std::shared_ptr<SecureEvidencePolicy> secureEvidencePolicy, std::shared_ptr<EvidencePolicyDescriptor> neutralEvidencePolicy, SecureEvidenceVisibilityClass evidenceClass)
	{
		if (secureEvidencePolicy == nullptr)
		{
			return Deny(SecureEvidencePublicationDecision::DeniedMissingSecureEvidencePolicy, L"Secure evidence publication requires a secure evidence policy.");
		}

		if (neutralEvidencePolicy == nullptr)
		{
			return Deny(SecureEvidencePublicationDecision::DeniedMissingNeutralEvidencePolicy, L"Secure evidence publication requires a neutral evidence policy.");
		}

		if (secureEvidencePolicy->MustQuarantine(evidenceClass))
		{
			return Deny(SecureEvidencePublicationDecision::DeniedHostOwnedEvidence, L"Host-owned or denied evidence cannot be published to guest-visible state.");
		}

		if (evidenceClass == SecureEvidenceVisibilityClass::RecomputedAfterRestore)
		{
			return Deny(SecureEvidencePublicationDecision::DeniedRecomputedEvidence, L"Recomputed-after-restore evidence cannot be reused as guest-visible publication state.");
		}

		if (evidenceClass == SecureEvidenceVisibilityClass::MigrationSerializable)
		{
			return Deny(SecureEvidencePublicationDecision::DeniedSecureEvidencePolicy, L"Migration-serializable evidence is not a guest-visible publication class.");
		}

		return SecureEvidencePublicationResult::Allowed;
	}

	std::shared_ptr<SecureEvidencePublicationResult> SecureEvidencePublicationPolicy::Deny(SecureEvidencePublicationDecision decision, const std::wstring &reason)
	{
		return SecureEvidencePublicationResult::Denied(decision, reason);
	}
}
