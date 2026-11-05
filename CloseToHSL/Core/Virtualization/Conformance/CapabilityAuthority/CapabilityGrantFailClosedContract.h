#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class CapabilityGrantFailClosedViolation : unsigned char
	{
		None = 0,
		MissingDescriptorSet = 1,
		MissingTypedGrantWasGranted = 2,
		OwnerAwareMissingGrantWasGranted = 3
	};


}
