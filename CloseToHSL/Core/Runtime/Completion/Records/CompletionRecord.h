#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class CompletionRecordClass : unsigned char
	{
		None = 0,
		Trap = 1,
		Event = 2,
		MemoryTranslationFault = 3,
		DmaFault = 4,
		VectorStreamFault = 5,
		LaneFault = 6,
		SecurityViolation = 7,
		CompatibilityExit = 8
	};


}
