#pragma once

#include "SecureComputeVmWriteDenyPolicy.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class SecureComputeVmWriteDenyPolicy final : public std::enable_shared_from_this<SecureComputeVmWriteDenyPolicy>
	{
	public:
		SecureComputeVmWriteDecision Deny(bool secureSensitiveField);
	};
}
