#pragma once

#include "Rf06ExecutionContracts.h"
#include <string>
#include <vector>
#include "tangible_exception_helper.h"
#include <memory>

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;
using namespace YAKSys_Hybrid_CPU::Core::Registers;

namespace YAKSys_Hybrid_CPU::Core::Decoder
{

	/// <summary>
	/// RF-08.3l creates only an immutable scalar-load contract source. It does not
	/// create an admission, schedule a lane, or observe dynamic memory state.
	/// </summary>
	class Rf08ScalarLoadContractProjection final
	{
	public:
		static inline const std::wstring PayloadSchema = L"scalar-load-unresolved-address-plan-v1";

		static std::shared_ptr<ExecutionContract> CreateContract(std::shared_ptr<CanonicalDecodedInstruction> canonical);
	};

}
