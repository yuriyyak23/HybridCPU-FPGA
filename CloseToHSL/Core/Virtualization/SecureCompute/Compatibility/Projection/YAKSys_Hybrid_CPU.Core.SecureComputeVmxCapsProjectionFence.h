#pragma once

#include "SecureComputeVmxCapsProjectionFence.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class SecureComputeVmxCapsProjectionFence final : public std::enable_shared_from_this<SecureComputeVmxCapsProjectionFence>
	{
	public:
		SecureComputeVmxCapsProjectionFenceDecision Validate(bool attemptsAuthorityGrant, bool attemptsActivation, bool attemptsWriteMutation);
	};
}
