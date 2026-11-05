#pragma once

#include "../../Compatibility/Frontend/Projection/Nested/NestedDomainController.h"

namespace YAKSys_Hybrid_CPU::Core::Nested
{

	enum class NestedCapabilityPublicationContractViolation : unsigned char
	{
		None = 0,
		MissingCompatibilityProjection = 1,
		MissingTypedAuthority = 2,
		MissingProjectedNestedCapability = 3,
		RequestBypassesPublicationAuthority = 4
	};


}

