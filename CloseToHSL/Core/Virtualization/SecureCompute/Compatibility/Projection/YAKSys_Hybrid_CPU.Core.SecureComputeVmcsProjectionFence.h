#pragma once

#include "SecureComputeVmcsProjectionFence.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class SecureComputeVmcsProjectionFence final : public std::enable_shared_from_this<SecureComputeVmcsProjectionFence>
	{
	public:
		SecureComputeVmcsProjectionFenceDecision Validate(bool storesSecureState, bool usedAsCheckpointAuthority);
	};
}
