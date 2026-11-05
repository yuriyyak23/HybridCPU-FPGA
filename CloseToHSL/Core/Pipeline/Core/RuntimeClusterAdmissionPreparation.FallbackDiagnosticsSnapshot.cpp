#include "RuntimeClusterAdmissionPreparation.FallbackDiagnosticsSnapshot.h"

namespace YAKSys_Hybrid_CPU::Core
{

	ClusterFallbackDiagnosticsSnapshot::ClusterFallbackDiagnosticsSnapshot(unsigned char fallbackDiagnosticsMask, bool suggestsFallbackDiagnostics, RuntimeClusterFallbackReasonMask fallbackReasonMask)
	{
		FallbackDiagnosticsMask = fallbackDiagnosticsMask;
		SuggestsFallbackDiagnostics = suggestsFallbackDiagnostics;
		FallbackReasonMask = fallbackReasonMask;
	}

	unsigned char ClusterFallbackDiagnosticsSnapshot::getFallbackDiagnosticsMask() const
	{
		return FallbackDiagnosticsMask;
	}

	bool ClusterFallbackDiagnosticsSnapshot::getSuggestsFallbackDiagnostics() const
	{
		return SuggestsFallbackDiagnostics;
	}

	RuntimeClusterFallbackReasonMask ClusterFallbackDiagnosticsSnapshot::getFallbackReasonMask() const
	{
		return FallbackReasonMask;
	}

	RuntimeClusterFallbackReasonMask ClusterFallbackDiagnosticsSnapshot::DeriveReasonMask(unsigned char fallbackDiagnosticsMask, unsigned char blockedScalarCandidateMask, unsigned char registerHazardMask, unsigned char orderingHazardMask)
	{
		RuntimeClusterFallbackReasonMask reasonMask = RuntimeClusterFallbackReasonMask::None;
		auto addReason = [&reasonMask](RuntimeClusterFallbackReasonMask reason)
		{
			reasonMask = static_cast<RuntimeClusterFallbackReasonMask>(static_cast<unsigned char>(reasonMask) | static_cast<unsigned char>(reason));
		};

		if (fallbackDiagnosticsMask != 0)
		{
			addReason(RuntimeClusterFallbackReasonMask::ResidualDiagnostics);
		}

		if (blockedScalarCandidateMask != 0)
		{
			addReason(RuntimeClusterFallbackReasonMask::BlockedScalarCandidates);
		}

		if (registerHazardMask != 0)
		{
			addReason(RuntimeClusterFallbackReasonMask::RegisterHazard);
		}

		if (orderingHazardMask != 0)
		{
			addReason(RuntimeClusterFallbackReasonMask::OrderingHazard);
		}

		return reasonMask;
	}
}
