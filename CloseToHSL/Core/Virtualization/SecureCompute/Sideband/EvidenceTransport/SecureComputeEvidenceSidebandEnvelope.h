#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class SecureComputeEvidenceSidebandClass : unsigned char
	{
		GuestVisible = 0,
		CompatibilityAlias = 1,
		HostOwnedQuarantined = 2,
		RecomputeAfterRestore = 3
	};


}
