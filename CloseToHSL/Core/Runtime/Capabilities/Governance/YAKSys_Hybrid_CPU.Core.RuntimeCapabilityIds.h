#pragma once

namespace YAKSys_Hybrid_CPU::Core
{
	class RuntimeCapabilityIds final
	{
	public:
		static constexpr int VmCallProbeNoStateV1Bit = 41;
		static constexpr unsigned long long VmCallProbeNoStateV1Mask = 1ULL << VmCallProbeNoStateV1Bit;
	};
}
