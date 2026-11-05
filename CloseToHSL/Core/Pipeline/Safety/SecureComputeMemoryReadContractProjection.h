#pragma once

#include "../../Decoder/Rf06ExecutionContracts.h"
#include "tangible_exception_helper.h"
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core::Decoder;

namespace YAKSys_Hybrid_CPU::Core
{

	class SecureComputeMemoryReadContractProjection final
	{
	public:
		static std::shared_ptr<ExecutionContract> Resolve(std::shared_ptr<CanonicalDecodedInstruction> canonical, std::shared_ptr<MemoryCapability> memory, int ownerThread);
	};

}
