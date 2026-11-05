#include "YAKSys_Hybrid_CPU.Core.SecureComputeMigrationReplayContract.h"

namespace YAKSys_Hybrid_CPU::Core
{

	SecureComputeMigrationReplayViolation SecureComputeMigrationReplayContract::Validate(bool epochRollback, bool vmcsProjectionAuthority, bool compatibilityMetadataAuthority, bool privateMemoryWithoutSealedPayload)
	{
		if (epochRollback)
		{
			return SecureComputeMigrationReplayViolation::EpochRollback;
		}

		if (vmcsProjectionAuthority)
		{
			return SecureComputeMigrationReplayViolation::VmcsProjectionAuthority;
		}

		if (compatibilityMetadataAuthority)
		{
			return SecureComputeMigrationReplayViolation::CompatibilityMetadataAuthority;
		}

		if (privateMemoryWithoutSealedPayload)
		{
			return SecureComputeMigrationReplayViolation::PrivateMemoryWithoutSealedPayload;
		}

		return SecureComputeMigrationReplayViolation::None;
	}
}
