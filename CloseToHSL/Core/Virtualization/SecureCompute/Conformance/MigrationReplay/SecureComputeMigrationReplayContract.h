#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class SecureComputeMigrationReplayViolation : unsigned char
	{
		None = 0,
		EpochRollback = 1,
		VmcsProjectionAuthority = 2,
		CompatibilityMetadataAuthority = 3,
		PrivateMemoryWithoutSealedPayload = 4
	};


}
