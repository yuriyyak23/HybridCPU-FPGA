#include "YAKSys_Hybrid_CPU.Core.SecureComputeVmxCapsProjectionFence.h"

namespace YAKSys_Hybrid_CPU::Core
{

	SecureComputeVmxCapsProjectionFenceDecision SecureComputeVmxCapsProjectionFence::Validate(bool attemptsAuthorityGrant, bool attemptsActivation, bool attemptsWriteMutation)
	{
		if (attemptsAuthorityGrant)
		{
			return SecureComputeVmxCapsProjectionFenceDecision::DeniedAuthorityGrant;
		}

		if (attemptsActivation)
		{
			return SecureComputeVmxCapsProjectionFenceDecision::DeniedActivationBit;
		}

		if (attemptsWriteMutation)
		{
			return SecureComputeVmxCapsProjectionFenceDecision::DeniedWriteMutation;
		}

		return SecureComputeVmxCapsProjectionFenceDecision::ProjectionAllowed;
	}
}
