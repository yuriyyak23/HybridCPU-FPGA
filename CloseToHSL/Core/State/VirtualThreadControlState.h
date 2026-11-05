#pragma once

#include "../Architecture/State/Pipeline/PipelineState.h"
#include <vector>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	/// <summary>
	/// Storage-only containment for the existing per-core virtual-thread FSM and
	/// VMX compatibility gates. Existing guarded transition, VMX execution and
	/// retire/control methods retain authority.
	/// </summary>
	class VirtualThreadControlState final : public std::enable_shared_from_this<VirtualThreadControlState>
	{
	public:
		bool IsVmxRoot = false;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
// ORIGINAL LINE: internal PipelineState[] PipelineStates = null!;
		std::vector<PipelineState> PipelineStates;
		bool VmxExecutionPlaneWired = false;
	};

}
