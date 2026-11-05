#pragma once

#include "RuntimeClusterAdmissionPreparation.FallbackDiagnosticsSnapshot.h"
#include "RuntimeClusterAdmissionPreparation.CandidateView.h"
#include "RuntimeClusterAdmissionPreparation.DecisionDraft.h"

namespace YAKSys_Hybrid_CPU::Core
{
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: internal readonly struct BundleIssueFallbackInfo
	class BundleIssueFallbackInfo
	{
	private:
		RuntimeClusterFallbackReasonMask FallbackReasonMask;
		unsigned char BlockedScalarCandidateMask;
		bool RetainsReferenceSequentialPath;

	public:
		BundleIssueFallbackInfo(RuntimeClusterFallbackReasonMask fallbackReasonMask, unsigned char blockedScalarCandidateMask, bool retainsReferenceSequentialPath);

		RuntimeClusterFallbackReasonMask getFallbackReasonMask() const;
		unsigned char getBlockedScalarCandidateMask() const;
		bool getRetainsReferenceSequentialPath() const;

		static BundleIssueFallbackInfo Create(RuntimeClusterAdmissionCandidateView candidateView, RuntimeClusterAdmissionDecisionDraft decisionDraft);

		static BundleIssueFallbackInfo CreateEmpty();

		BundleIssueFallbackInfo() = default;
	};
}
