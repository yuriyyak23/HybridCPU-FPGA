#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class SecureComputeVmxCapsProjectionFenceDecision : unsigned char
	{
		ProjectionAllowed = 0,
		DeniedAuthorityGrant = 1,
		DeniedActivationBit = 2,
		DeniedWriteMutation = 3
	};


}
