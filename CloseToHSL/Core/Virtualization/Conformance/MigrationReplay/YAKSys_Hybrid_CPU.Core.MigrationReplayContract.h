#pragma once

#include "MigrationReplayContract.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class MigrationReplayContract final : public std::enable_shared_from_this<MigrationReplayContract>
	{
	public:
		std::shared_ptr<MigrationReplayContractResult> ValidateReplay(std::shared_ptr<MigrationReplayContractRequest> request);
		bool CanReplay(std::shared_ptr<MigrationReplayContractRequest> request);

	private:
		static bool IsHostOwnedPayload(MigrationPayloadClass payloadClass);
		static bool IsHostOwnedEvidence(EvidenceVisibilityClass evidenceClass);
	};
}
