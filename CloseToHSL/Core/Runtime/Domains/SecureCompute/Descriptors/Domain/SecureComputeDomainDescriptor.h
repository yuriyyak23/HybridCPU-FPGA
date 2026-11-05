#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class SecureComputeSecurityLevel : unsigned char
	{
		Disabled = 0,
		None = 0,
		Measured = 1,
		Private = 2,
		RestrictedInspection = 3,
		SealedRuntime = 4
	};


}
