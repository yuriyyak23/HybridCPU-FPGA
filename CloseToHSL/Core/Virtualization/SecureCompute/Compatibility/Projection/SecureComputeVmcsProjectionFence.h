#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class SecureComputeVmcsProjectionFenceDecision : unsigned char
	{
		ProjectionAllowed = 0,
		DeniedSecureStateStore = 1,
		DeniedCheckpointAuthority = 2
	};


}
