#pragma once

#include "../../Compatibility/Frontend/Projection/Nested/NestedDomainController.h"


namespace YAKSys_Hybrid_CPU::Core
{

	enum class NestedDomainCapabilityProjectionViolation : unsigned char
	{
		None = 0,
		MissingGenericCapability = 1,
		MissingFrontendProjectionMask = 2,
		MissingOwnerDomain = 3,
		MissingTypedAuthority = 4
	};


}

