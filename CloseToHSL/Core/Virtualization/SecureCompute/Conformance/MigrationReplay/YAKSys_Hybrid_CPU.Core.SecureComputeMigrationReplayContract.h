#pragma once

#include "SecureComputeMigrationReplayContract.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class SecureComputeMigrationReplayContract final : public std::enable_shared_from_this<SecureComputeMigrationReplayContract>
	{
	public:
		SecureComputeMigrationReplayViolation Validate(bool epochRollback, bool vmcsProjectionAuthority, bool compatibilityMetadataAuthority, bool privateMemoryWithoutSealedPayload);
	};
}
