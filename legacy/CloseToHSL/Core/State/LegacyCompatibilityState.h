#pragma once

#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	/// <summary>
	/// Storage-only containment for the retained legacy FSM counter/flag surface.
	/// This state is distinct from PipelineControl and modern telemetry counters;
	/// RF-11 containment does not authorize RF-13 removal or reinterpretation.
	/// </summary>
	class LegacyCompatibilityState final : public std::enable_shared_from_this<LegacyCompatibilityState>
	{
	public:
		unsigned long long CycleCounter = 0;
		int StageCycleCounter = 0;
		bool Stalled = false;
	};

}
