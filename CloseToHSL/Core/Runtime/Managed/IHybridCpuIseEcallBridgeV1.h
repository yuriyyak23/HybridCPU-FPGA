#pragma once

#include "../../Architecture/Registers/CSR/CsrAccessPolicy.h"
#include "../../State/ICpuState.h"
#include "../../Pipeline/PipelineEvent.h"
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core::Pipeline;
using namespace YAKSys_Hybrid_CPU::Core::Registers;

namespace YAKSys_Hybrid_CPU::Core
{

	/// <summary>Explicit production hook for trusted user ECALL handling at retirement.</summary>
	class IHybridCpuIseEcallBridgeV1
	{
	public:
		virtual bool TryHandle(std::shared_ptr<EcallEvent> ecall, std::shared_ptr<ICanonicalCpuState> state, PrivilegeLevel privilege) = 0;
	};

}
