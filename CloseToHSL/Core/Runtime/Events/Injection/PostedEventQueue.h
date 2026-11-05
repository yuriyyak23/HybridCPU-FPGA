#pragma once
#include "EventInjectionDescriptor.h"

#include <vector>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class PostedEventQueueSnapshot final : public std::enable_shared_from_this<PostedEventQueueSnapshot>
	{
	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly IReadOnlyList<EventInjectionDescriptor> PendingEvents;
		const std::vector<std::shared_ptr<EventInjectionDescriptor>> PendingEvents;
		const unsigned long long NextSequence;
		const unsigned long long InvalidDroppedCount;
		const unsigned long long CapacityDroppedCount;

	public:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public PostedEventQueueSnapshot(IReadOnlyList<EventInjectionDescriptor> pendingEvents, ulong nextSequence, ulong invalidDroppedCount, ulong capacityDroppedCount)
		PostedEventQueueSnapshot(const std::vector<std::shared_ptr<EventInjectionDescriptor>> &pendingEvents, unsigned long long nextSequence, unsigned long long invalidDroppedCount, unsigned long long capacityDroppedCount);

		const std::vector<std::shared_ptr<EventInjectionDescriptor>> getPendingEvents() const;

		unsigned long long getNextSequence() const;

		unsigned long long getInvalidDroppedCount() const;

		unsigned long long getCapacityDroppedCount() const;
	};

}
