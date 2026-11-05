#include "NeutralArchitecturalCompletionFacts.h"

namespace YAKSys_Hybrid_CPU::Core
{

	unsigned long long ArchitecturalCompletionEventIdentity::Create(unsigned long long bundleSequence, int slotIndex)
	{
		if (bundleSequence == 0 || static_cast<unsigned int>(slotIndex) >= BundleSlotCount)
		{
			return 0;
		}

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: ulong identity = unchecked((bundleSequence * 0x9E3779B185EBCA87UL) ^ ((ulong)(uint)slotIndex + 0xD1B54A32D192ED03UL));
		unsigned long long identity = (bundleSequence * 0x9E3779B185EBCA87ULL) ^ (static_cast<unsigned long long>(static_cast<unsigned int>(slotIndex)) + 0xD1B54A32D192ED03ULL);
		return identity == 0 ? 1ULL : identity;
	}
}
