#pragma once

#include "../../Compatibility/Frontend/Projection/Nested/NestedDomainController.h"


namespace YAKSys_Hybrid_CPU::Core
{

	enum class NestedEnablementProofAuthorityViolation : unsigned char
	{
		None = 0,
		MissingRequiredCapabilityProof = 1,
		MissingRequiredGateProof = 2,
		NonAuthoritativeProof = 3
	};


}

