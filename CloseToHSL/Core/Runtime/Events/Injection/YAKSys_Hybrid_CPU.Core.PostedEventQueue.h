#pragma once

#include "InterruptRemapPolicy.h"
#include "PostedEventQueue.h"
#include <vector>
#include <deque>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class PostedEventQueue final : public std::enable_shared_from_this<PostedEventQueue>
	{
	private:
		unsigned long long InvalidDroppedCount = 0;
		unsigned long long CapacityDroppedCount = 0;

		const int _capacity;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Queue<EventInjectionDescriptor> _queue = new();
		std::deque<std::shared_ptr<EventInjectionDescriptor>> _queue = std::deque<std::shared_ptr<EventInjectionDescriptor>>();
		unsigned long long _nextSequence = 0;

	public:
		PostedEventQueue(int capacity = 64);

		int getCount() const;

		unsigned long long getDroppedCount() const;

		unsigned long long getInvalidDroppedCount() const;
	private:
		void setInvalidDroppedCount(unsigned long long value);

	public:
		unsigned long long getCapacityDroppedCount() const;
	private:
		void setCapacityDroppedCount(unsigned long long value);

	public:
		bool TryEnqueue(std::shared_ptr<EventInjectionDescriptor> descriptor, std::shared_ptr<EventInjectionDescriptor> &queued);

		bool TryEnqueue(std::shared_ptr<EventInjectionDescriptor> descriptor, std::shared_ptr<EventInjectionDescriptor> &queued, InterruptPostDisposition &disposition);

		bool TryPeek(std::shared_ptr<EventInjectionDescriptor> &descriptor);

		bool TryFindCoalescingMatch(std::shared_ptr<EventInjectionDescriptor> descriptor, std::shared_ptr<EventInjectionDescriptor> &existing);

		bool TryDequeue(std::shared_ptr<EventInjectionDescriptor> &descriptor);

		bool TryDequeueMatching(unsigned char targetVtId, unsigned short executionDomainTag, bool preferPriority, std::shared_ptr<EventInjectionDescriptor> &descriptor);



		std::shared_ptr<PostedEventQueueSnapshot> CreateSnapshot();

		void RestoreSnapshot(std::shared_ptr<PostedEventQueueSnapshot> snapshot);
	};
}
