#include "PostedEventQueue.h"

namespace YAKSys_Hybrid_CPU::Core
{

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public PostedEventQueueSnapshot(IReadOnlyList<EventInjectionDescriptor> pendingEvents, ulong nextSequence, ulong invalidDroppedCount, ulong capacityDroppedCount)
	PostedEventQueueSnapshot::PostedEventQueueSnapshot(const std::vector<std::shared_ptr<EventInjectionDescriptor>> &pendingEvents, unsigned long long nextSequence, unsigned long long invalidDroppedCount, unsigned long long capacityDroppedCount) : PendingEvents(pendingEvents), NextSequence(nextSequence), InvalidDroppedCount(invalidDroppedCount), CapacityDroppedCount(capacityDroppedCount)
	{
	}

	const std::vector<std::shared_ptr<EventInjectionDescriptor>> PostedEventQueueSnapshot::getPendingEvents() const
	{
		return PendingEvents;
	}

	unsigned long long PostedEventQueueSnapshot::getNextSequence() const
	{
		return NextSequence;
	}

	unsigned long long PostedEventQueueSnapshot::getInvalidDroppedCount() const
	{
		return InvalidDroppedCount;
	}

	unsigned long long PostedEventQueueSnapshot::getCapacityDroppedCount() const
	{
		return CapacityDroppedCount;
	}
}
