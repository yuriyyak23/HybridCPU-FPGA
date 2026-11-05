#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class CompatibilityCapsProjectionViolation : unsigned char
	{
		None = 0,
		MissingDescriptorSet = 1,
		ProjectionMismatch = 2,
		ProjectionBypassesTypedGrants = 3
	};


}
