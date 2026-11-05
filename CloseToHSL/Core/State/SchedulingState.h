#pragma once

#include "../Pipeline/Assist/InterCore/YAKSys_Hybrid_CPU.Core.MicroOpScheduler.h"
#include <vector>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	/// <summary>
	/// Core-local scheduling bindings and VT selection/stall state. The referenced
	/// MicroOpScheduler retains ownership of all queues, legality, fairness,
	/// scoreboard, FSP, replay and assist internals.
	/// </summary>
	class SchedulingState final : public std::enable_shared_from_this<SchedulingState>
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
// ORIGINAL LINE: internal MicroOpScheduler Scheduler = null!;
	public:
		std::shared_ptr<MicroOpScheduler> Scheduler = nullptr;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
// ORIGINAL LINE: internal bool[] VirtualThreadStalled = null!;
		std::vector<bool> VirtualThreadStalled;
		int ActiveVirtualThreadId = 0;
	};

}
