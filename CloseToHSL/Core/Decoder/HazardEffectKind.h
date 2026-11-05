#pragma once

namespace YAKSys_Hybrid_CPU::Core
{
enum class HazardEffectKind : unsigned char
	{
		RegisterData = 0,
		MemoryBank = 1,
		ControlFlow = 2,
		SystemBarrier = 3,
		PinnedLane = 4
	};
}
