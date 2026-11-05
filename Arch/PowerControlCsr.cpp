#include "PowerControlCsr.h"
#include "CsrAddresses.h"

namespace YAKSys_Hybrid_CPU::Arch
{

	bool PowerControlCsr::IsPerformanceState(Processor::CPU_Core::CorePowerState state)
	{
		return state >= Processor::CPU_Core::CorePowerState::P0_MaxPerformance && state <= Processor::CPU_Core::CorePowerState::Pn_MinPerformance;
	}

	unsigned short PowerControlCsr::ResolveTargetCsr(Processor::CPU_Core::CorePowerState state)
	{
		return IsPerformanceState(state) ? CsrAddresses::MperfLevel : CsrAddresses::MpowerState;
	}

	unsigned long long PowerControlCsr::EncodeState(Processor::CPU_Core::CorePowerState state)
	{
		return static_cast<unsigned long long>(static_cast<unsigned int>(state));
	}

	Processor::CPU_Core::CorePowerState PowerControlCsr::DecodePowerState(unsigned long long rawValue)
	{
        using State = Processor::CPU_Core::CorePowerState;
        switch (rawValue) {
            case static_cast<unsigned long long>(State::C0_Active): return State::C0_Active;
            case static_cast<unsigned long long>(State::C1_Halt): return State::C1_Halt;
            case static_cast<unsigned long long>(State::C2_StopClock): return State::C2_StopClock;
            case static_cast<unsigned long long>(State::C3_Sleep): return State::C3_Sleep;
            case static_cast<unsigned long long>(State::C6_DeepPowerDown): return State::C6_DeepPowerDown;
            default: return State::ErrorState;
        }
    }

	unsigned int PowerControlCsr::DecodePerformanceLevel(unsigned long long rawValue)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((uint)rawValue);
		return static_cast<unsigned int>(rawValue);
	}
}
