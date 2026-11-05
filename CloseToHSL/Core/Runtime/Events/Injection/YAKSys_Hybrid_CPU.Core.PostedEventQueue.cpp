#include "YAKSys_Hybrid_CPU.Core.PostedEventQueue.h"

namespace YAKSys_Hybrid_CPU::Core
{

	PostedEventQueue::PostedEventQueue(int capacity) : _capacity(capacity <= 0 ? 1 : capacity)
	{
	}

	int PostedEventQueue::getCount() const
	{
		return _queue.size();
	}

	unsigned long long PostedEventQueue::getDroppedCount() const
	{
		return getInvalidDroppedCount() + getCapacityDroppedCount();
	}

	unsigned long long PostedEventQueue::getInvalidDroppedCount() const
	{
		return InvalidDroppedCount;
	}

	void PostedEventQueue::setInvalidDroppedCount(unsigned long long value)
	{
		InvalidDroppedCount = value;
	}

	unsigned long long PostedEventQueue::getCapacityDroppedCount() const
	{
		return CapacityDroppedCount;
	}

	void PostedEventQueue::setCapacityDroppedCount(unsigned long long value)
	{
		CapacityDroppedCount = value;
	}

	bool PostedEventQueue::TryEnqueue(std::shared_ptr<EventInjectionDescriptor> descriptor, std::shared_ptr<EventInjectionDescriptor> &queued)
	{
		InterruptPostDisposition ignored; return TryEnqueue(descriptor, queued, ignored);
	}

	bool PostedEventQueue::TryEnqueue(std::shared_ptr<EventInjectionDescriptor> descriptor, std::shared_ptr<EventInjectionDescriptor> &queued, InterruptPostDisposition &disposition)
	{
		if (!descriptor || !descriptor->IsValid())
		{
			queued = nullptr;
			setInvalidDroppedCount(getInvalidDroppedCount() + 1);
			disposition = InterruptPostDisposition::DroppedInvalid;
			return false;
		}

		if (_queue.size() >= _capacity)
		{
			queued = nullptr;
			setCapacityDroppedCount(getCapacityDroppedCount() + 1);
			disposition = InterruptPostDisposition::DroppedQueueFull;
			return false;
		}

//		unchecked
			_nextSequence++;
// C# TO C++ CONVERTER TASK: End of the original C# 'unchecked' block.

		queued = descriptor->WithSequence(_nextSequence == 0 ? 1 : _nextSequence);
		_queue.push_back(queued);
		disposition = InterruptPostDisposition::Queued;
		return true;
	}

	bool PostedEventQueue::TryPeek(std::shared_ptr<EventInjectionDescriptor> &descriptor)
	{
		if (_queue.empty()) { descriptor.reset(); return false; } descriptor = _queue.front(); return true;
	}

	bool PostedEventQueue::TryFindCoalescingMatch(std::shared_ptr<EventInjectionDescriptor> descriptor, std::shared_ptr<EventInjectionDescriptor> &existing)
	{
		for (auto candidate : _queue)
		{
			if (candidate->CoalescesWith(*descriptor))
			{
				existing = candidate;
				return true;
			}
		}

		existing = nullptr;
		return false;
	}

	bool PostedEventQueue::TryDequeue(std::shared_ptr<EventInjectionDescriptor> &descriptor)
	{
		if (_queue.empty())
		{
			descriptor = nullptr;
			return false;
		}

		descriptor = _queue.front(); _queue.pop_front();
		return true;
	}

	bool PostedEventQueue::TryDequeueMatching(unsigned char targetVtId, unsigned short executionDomainTag, bool preferPriority, std::shared_ptr<EventInjectionDescriptor> &descriptor)
	{
		descriptor = nullptr;
		if (_queue.empty())
		{
			return false;
		}

		int count = _queue.size();
		bool found = false;
		std::shared_ptr<EventInjectionDescriptor> selected = nullptr;
		std::deque<std::shared_ptr<EventInjectionDescriptor>> retained;

		for (int i = 0; i < count; i++)
		{
			std::shared_ptr<EventInjectionDescriptor> candidate = _queue.front(); _queue.pop_front();
			if (!candidate->Matches(targetVtId, executionDomainTag))
			{
				retained.push_back(candidate);
				continue;
			}

			if (!found || (preferPriority && candidate->Priority > selected->Priority) || (preferPriority && candidate->Priority == selected->Priority && candidate->Sequence < selected->Sequence))
			{
				if (found)
				{
					retained.push_back(selected);
				}

				selected = candidate;
				found = true;
				continue;
			}

			retained.push_back(candidate);
		}

		while (retained.size() != 0)
		{
			_queue.push_back(retained.front()); retained.pop_front();
		}

		if (!found)
		{
			return false;
		}

		descriptor = selected;
		return true;
	}

	std::shared_ptr<PostedEventQueueSnapshot> PostedEventQueue::CreateSnapshot()
	{
		return std::make_shared<PostedEventQueueSnapshot>(std::vector<std::shared_ptr<EventInjectionDescriptor>>(_queue.begin(), _queue.end()), _nextSequence, getInvalidDroppedCount(), getCapacityDroppedCount());
	}

	void PostedEventQueue::RestoreSnapshot(std::shared_ptr<PostedEventQueueSnapshot> snapshot)
	{
		_queue.clear();
		for (auto descriptor : snapshot->getPendingEvents())
		{
			if (descriptor && descriptor->IsValid())
			{
				_queue.push_back(descriptor);
			}
		}

		_nextSequence = snapshot->getNextSequence();
		setInvalidDroppedCount(snapshot->getInvalidDroppedCount());
		setCapacityDroppedCount(snapshot->getCapacityDroppedCount());
	}
}
