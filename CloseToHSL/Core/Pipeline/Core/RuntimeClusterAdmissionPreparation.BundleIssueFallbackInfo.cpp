#include "RuntimeClusterAdmissionPreparation.BundleIssueFallbackInfo.h"

namespace YAKSys_Hybrid_CPU::Core
{

	BundleIssueFallbackInfo::BundleIssueFallbackInfo(RuntimeClusterFallbackReasonMask fallbackReasonMask, unsigned char blockedScalarCandidateMask, bool retainsReferenceSequentialPath)
	{
		FallbackReasonMask = fallbackReasonMask;
		BlockedScalarCandidateMask = blockedScalarCandidateMask;
		RetainsReferenceSequentialPath = retainsReferenceSequentialPath;
	}

	RuntimeClusterFallbackReasonMask BundleIssueFallbackInfo::getFallbackReasonMask() const
	{
		return FallbackReasonMask;
	}

	unsigned char BundleIssueFallbackInfo::getBlockedScalarCandidateMask() const
	{
		return BlockedScalarCandidateMask;
	}

	bool BundleIssueFallbackInfo::getRetainsReferenceSequentialPath() const
	{
		return RetainsReferenceSequentialPath;
	}

	BundleIssueFallbackInfo BundleIssueFallbackInfo::Create(RuntimeClusterAdmissionCandidateView candidateView, RuntimeClusterAdmissionDecisionDraft decisionDraft)
	{
		return BundleIssueFallbackInfo(decisionDraft.getFallbackReasonMask(), candidateView.getBlockedScalarCandidateMask(), decisionDraft.getRetainsReferenceSequentialPath());
	}

	BundleIssueFallbackInfo BundleIssueFallbackInfo::CreateEmpty()
	{
		return BundleIssueFallbackInfo(RuntimeClusterFallbackReasonMask::None, 0, true);
	}
}
