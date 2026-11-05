#pragma once

namespace HybridCPU_ISE::Machine
{
	enum class MachineStateSourceProvenance
	{
		LiveCore = 0,
		Snapshot = 1,
		LegacyGlobal = 2,
		Null = 3
	};
}
