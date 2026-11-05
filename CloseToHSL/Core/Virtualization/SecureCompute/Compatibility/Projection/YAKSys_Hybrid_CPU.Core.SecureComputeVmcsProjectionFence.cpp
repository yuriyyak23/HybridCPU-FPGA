#include "YAKSys_Hybrid_CPU.Core.SecureComputeVmcsProjectionFence.h"

namespace YAKSys_Hybrid_CPU::Core
{

	SecureComputeVmcsProjectionFenceDecision SecureComputeVmcsProjectionFence::Validate(bool storesSecureState, bool usedAsCheckpointAuthority)
	{
		if (storesSecureState)
		{
			return SecureComputeVmcsProjectionFenceDecision::DeniedSecureStateStore;
		}

		if (usedAsCheckpointAuthority)
		{
			return SecureComputeVmcsProjectionFenceDecision::DeniedCheckpointAuthority;
		}

		return SecureComputeVmcsProjectionFenceDecision::ProjectionAllowed;
	}
}
