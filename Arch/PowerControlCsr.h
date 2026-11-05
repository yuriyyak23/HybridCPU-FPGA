#pragma once

#include "../CloseToHSL/Core/Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"

namespace YAKSys_Hybrid_CPU::Arch
{
	/// <summary>
	/// Canonical CSR routing helpers for core-local power-control state.
	/// </summary>
	class PowerControlCsr final
	{
	public:
		static bool IsPerformanceState(Processor::CPU_Core::CorePowerState state);

		static unsigned short ResolveTargetCsr(Processor::CPU_Core::CorePowerState state);

		static unsigned long long EncodeState(Processor::CPU_Core::CorePowerState state);

		static Processor::CPU_Core::CorePowerState DecodePowerState(unsigned long long rawValue);

		static unsigned int DecodePerformanceLevel(unsigned long long rawValue);
	};
}
