#include "MemoryDomainTranslationControl.h"

using namespace YAKSys_Hybrid_CPU::Memory;

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<RuntimeEpochAdvanceResult> MemoryDomainTranslationEpoch::TryAdvanceFailClosed(unsigned long long current)
	{
		if (current == std::numeric_limits<unsigned long long>::max())
		{
			return RuntimeEpochAdvanceResult::DeniedWraparound(current);
		}

		return RuntimeEpochAdvanceResult::Advanced(current + 1);
	}
}
