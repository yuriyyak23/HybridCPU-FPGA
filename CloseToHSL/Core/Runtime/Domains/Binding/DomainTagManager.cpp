#include "DomainTagManager.h"

namespace YAKSys_Hybrid_CPU::Core
{

	unsigned long long DomainTagManager::Issue()
	{
		// Atomic post-increment. Wrap-around at ulong.MaxValue+1 would produce 0,
		// which is the reserved NoTag sentinel. Skip it to maintain the invariant
		// that Issue() never returns 0.
		unsigned long long tag;
		do
		{
			tag = _counter.fetch_add(1, std::memory_order_relaxed) + 1;
		} while (tag == NoTag);
		return tag;
	}

	void DomainTagManager::Reset()
	{
		_counter.store(0, std::memory_order_relaxed);
	}
}
