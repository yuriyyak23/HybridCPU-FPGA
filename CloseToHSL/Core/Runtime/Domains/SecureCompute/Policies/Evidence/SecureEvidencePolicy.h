#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class SecureEvidenceVisibilityClass : unsigned char
	{
		Denied = 0,
		GuestVisible = 1,
		MigrationSerializable = 2,
		CompatibilityAlias = 3,
		RecomputedAfterRestore = 4,
		DebugOnly = 5,
		HostOwnedQuarantined = 6
	};


}
