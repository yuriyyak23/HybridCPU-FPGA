#pragma once

#include "SecureComputeNoEmissionContract.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class SecureComputeNoEmissionContract final : public std::enable_shared_from_this<SecureComputeNoEmissionContract>
	{
	public:
		SecureComputeNoEmissionViolation Validate(bool emitsNewInstructionEncoding, bool emitsNewOperandFormat, bool emitsCapabilityAwareLoadStoreFetch, bool emitsVmxSecureMode);
	};
}
