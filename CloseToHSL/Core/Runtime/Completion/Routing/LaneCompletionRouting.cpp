#include "LaneCompletionRouting.h"
#include "../../Events/Injection/InterruptRemapPolicy.h"
#include "../../Events/Injection/EventInjectionDescriptor.h"

namespace YAKSys_Hybrid_CPU::Core
{

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public LaneCompletionRouterSnapshot(IReadOnlyList<LaneCompletionRoute> routes, ulong routingEpoch, ulong routedCount, ulong ignoredDisabledCount, ulong invalidDroppedCount, ulong capacityDroppedCount, ulong eventQueueDroppedCount)
	LaneCompletionRouterSnapshot::LaneCompletionRouterSnapshot(const std::vector<std::shared_ptr<LaneCompletionRoute>> &routes, unsigned long long routingEpoch, unsigned long long routedCount, unsigned long long ignoredDisabledCount, unsigned long long invalidDroppedCount, unsigned long long capacityDroppedCount, unsigned long long eventQueueDroppedCount) : Routes(routes), RoutingEpoch(routingEpoch), RoutedCount(routedCount), IgnoredDisabledCount(ignoredDisabledCount), InvalidDroppedCount(invalidDroppedCount), CapacityDroppedCount(capacityDroppedCount), EventQueueDroppedCount(eventQueueDroppedCount)
	{
	}

	const std::vector<std::shared_ptr<LaneCompletionRoute>> LaneCompletionRouterSnapshot::getRoutes() const
	{
		return Routes;
	}

	unsigned long long LaneCompletionRouterSnapshot::getRoutingEpoch() const
	{
		return RoutingEpoch;
	}

	unsigned long long LaneCompletionRouterSnapshot::getRoutedCount() const
	{
		return RoutedCount;
	}

	unsigned long long LaneCompletionRouterSnapshot::getIgnoredDisabledCount() const
	{
		return IgnoredDisabledCount;
	}

	unsigned long long LaneCompletionRouterSnapshot::getInvalidDroppedCount() const
	{
		return InvalidDroppedCount;
	}

	unsigned long long LaneCompletionRouterSnapshot::getCapacityDroppedCount() const
	{
		return CapacityDroppedCount;
	}

	unsigned long long LaneCompletionRouterSnapshot::getEventQueueDroppedCount() const
	{
		return EventQueueDroppedCount;
	}

	LaneCompletionQueue::LaneCompletionQueue(int capacity) : _capacity(capacity <= 0 ? 1 : capacity)
	{
	}

	int LaneCompletionQueue::getCount() const
	{
		return _queue.size();
	}

	unsigned long long LaneCompletionQueue::getInvalidDroppedCount() const
	{
		return InvalidDroppedCount;
	}

	void LaneCompletionQueue::setInvalidDroppedCount(unsigned long long value)
	{
		InvalidDroppedCount = value;
	}

	unsigned long long LaneCompletionQueue::getCapacityDroppedCount() const
	{
		return CapacityDroppedCount;
	}

	void LaneCompletionQueue::setCapacityDroppedCount(unsigned long long value)
	{
		CapacityDroppedCount = value;
	}

	bool LaneCompletionQueue::TryEnqueue(std::shared_ptr<LaneCompletionDescriptor> descriptor, std::shared_ptr<LaneCompletionDescriptor> &queued, LaneCompletionRouteStatus &status)
	{
		queued = nullptr;
		if (!descriptor || !descriptor->IsValid())
		{
			setInvalidDroppedCount(getInvalidDroppedCount() + 1);
			status = LaneCompletionRouteStatus::DroppedInvalid;
			return false;
		}

		if (_queue.size() >= _capacity)
		{
			setCapacityDroppedCount(getCapacityDroppedCount() + 1);
			status = LaneCompletionRouteStatus::DroppedQueueFull;
			return false;
		}

//		unchecked
			_nextSequence++;
			if (_nextSequence == 0)
			{
				_nextSequence = 1;
			}
// C# TO C++ CONVERTER TASK: End of the original C# 'unchecked' block.

		queued = descriptor->WithSequence(_nextSequence);
		_queue.push_back(queued);
		status = LaneCompletionRouteStatus::Routed;
		return true;
	}

	bool LaneCompletionQueue::TryDequeue(std::shared_ptr<LaneCompletionDescriptor> &descriptor)
	{
		if (_queue.empty())
		{
			descriptor = nullptr;
			return false;
		}

		descriptor = _queue.front(); _queue.pop_front();
		return true;
	}

	void LaneCompletionQueue::RestoreCounters(unsigned long long invalidDroppedCount, unsigned long long capacityDroppedCount)
	{
		_queue.clear();
		setInvalidDroppedCount(invalidDroppedCount);
		setCapacityDroppedCount(capacityDroppedCount);
		_nextSequence = 0;
	}

	LaneCompletionRouter::LaneCompletionRouter(int capacity) : _queue(std::make_shared<LaneCompletionQueue>(capacity))
	{
	}

	int LaneCompletionRouter::getPendingCount() const
	{
		return _queue->getCount();
	}

	unsigned long long LaneCompletionRouter::getRoutingEpoch() const
	{
		return RoutingEpoch;
	}

	void LaneCompletionRouter::setRoutingEpoch(unsigned long long value)
	{
		RoutingEpoch = value;
	}

	unsigned long long LaneCompletionRouter::getRoutedCount() const
	{
		return RoutedCount;
	}

	void LaneCompletionRouter::setRoutedCount(unsigned long long value)
	{
		RoutedCount = value;
	}

	unsigned long long LaneCompletionRouter::getIgnoredDisabledCount() const
	{
		return IgnoredDisabledCount;
	}

	void LaneCompletionRouter::setIgnoredDisabledCount(unsigned long long value)
	{
		IgnoredDisabledCount = value;
	}

	unsigned long long LaneCompletionRouter::getInvalidDroppedCount() const
	{
		return _queue->getInvalidDroppedCount();
	}

	unsigned long long LaneCompletionRouter::getCapacityDroppedCount() const
	{
		return _queue->getCapacityDroppedCount();
	}

	unsigned long long LaneCompletionRouter::getEventQueueDroppedCount() const
	{
		return EventQueueDroppedCount;
	}

	void LaneCompletionRouter::setEventQueueDroppedCount(unsigned long long value)
	{
		EventQueueDroppedCount = value;
	}

	void LaneCompletionRouter::ConfigureRoute(std::shared_ptr<LaneCompletionRoute> route)
	{
		if (route->LaneIndex >= _routes.size())
		{
			return;
		}

		_routes[route->LaneIndex] = route;
		AdvanceRoutingEpoch();
	}

	void LaneCompletionRouter::DisableRoute(unsigned char laneIndex)
	{
		if (laneIndex >= _routes.size())
		{
			return;
		}

		_routes[laneIndex] = LaneCompletionRoute::Disabled(laneIndex);
		AdvanceRoutingEpoch();
	}

	bool LaneCompletionRouter::TryBuildPostedCompletion(std::shared_ptr<LaneCompletionDescriptor> descriptor, std::shared_ptr<EventInjectionDescriptor> &evt, std::shared_ptr<LaneCompletionRoutingResult> &result)
	{
		evt = nullptr;
		if (!descriptor || !descriptor->IsValid())
		{
			std::shared_ptr<LaneCompletionDescriptor> invalid;
			LaneCompletionRouteStatus ignored; _queue->TryEnqueue(descriptor, invalid, ignored);
			AdvanceRoutingEpoch();
			result = LaneCompletionRoutingResult::Create(LaneCompletionRouteStatus::DroppedInvalid, invalid, evt, InterruptPostDisposition::DroppedInvalid, getRoutingEpoch(), L"Lane completion descriptor was invalid before compatibility completion routing.");
			return false;
		}

		std::shared_ptr<LaneCompletionRoute> route = _routes[descriptor->LaneIndex];
		if (!route->Matches(descriptor))
		{
			setIgnoredDisabledCount(getIgnoredDisabledCount() + 1);
			AdvanceRoutingEpoch();
			result = LaneCompletionRoutingResult::Create(LaneCompletionRouteStatus::IgnoredRouteDisabled, descriptor, evt, InterruptPostDisposition::None, getRoutingEpoch(), L"Lane completion routing is disabled or does not match the completion owner.");
			return false;
		}

		std::shared_ptr<LaneCompletionDescriptor> queued;
		LaneCompletionRouteStatus queueStatus;
		if (!_queue->TryEnqueue(descriptor, queued, queueStatus))
		{
			AdvanceRoutingEpoch();
			result = LaneCompletionRoutingResult::Create(queueStatus, queued, evt, queueStatus == LaneCompletionRouteStatus::DroppedQueueFull ? InterruptPostDisposition::DroppedQueueFull : InterruptPostDisposition::DroppedInvalid, getRoutingEpoch(), L"Lane completion queue rejected the descriptor before posted-event materialization.");
			return false;
		}

		_queue->TryDequeue(queued);
		unsigned long long payload = queued->EncodePayload();
		if (!route->CoalescePosted)
		{
			payload ^= queued->Sequence << 1;
		}

		evt = EventInjectionDescriptor::Create(EventInjectionKind::PostedCompletion, route->Vector, route->TargetVtId, route->ExecutionDomainTag, route->AddressSpaceTag, route->Priority, payload, true);

		AdvanceRoutingEpoch();
		result = LaneCompletionRoutingResult::Create(LaneCompletionRouteStatus::Routed, queued, evt, InterruptPostDisposition::None, getRoutingEpoch(), L"Lane completion descriptor was queued and materialized as a posted compatibility completion event.");
		return true;
	}

	void LaneCompletionRouter::RecordEventQueueDrop()
	{
		setEventQueueDroppedCount(getEventQueueDroppedCount() + 1);
		AdvanceRoutingEpoch();
	}

	void LaneCompletionRouter::RecordRouted()
	{
		setRoutedCount(getRoutedCount() + 1);
		AdvanceRoutingEpoch();
	}

	std::shared_ptr<LaneCompletionRouterSnapshot> LaneCompletionRouter::CreateSnapshot()
	{
		return std::make_shared<LaneCompletionRouterSnapshot>(std::vector<std::shared_ptr<LaneCompletionRoute>>(_routes), getRoutingEpoch(), getRoutedCount(), getIgnoredDisabledCount(), getInvalidDroppedCount(), getCapacityDroppedCount(), getEventQueueDroppedCount());
	}

	void LaneCompletionRouter::RestoreSnapshot(std::shared_ptr<LaneCompletionRouterSnapshot> snapshot)
	{
		int count = snapshot->getRoutes().size() < _routes.size() ? snapshot->getRoutes().size() : _routes.size();
		for (int index = 0; index < _routes.size(); index++)
		{
			_routes[index] = index < count ? snapshot->getRoutes()[index] : LaneCompletionRoute::Disabled(static_cast<unsigned char>(index));
		}

		_queue->RestoreCounters(snapshot->getInvalidDroppedCount(), snapshot->getCapacityDroppedCount());
		setRoutingEpoch(snapshot->getRoutingEpoch());
		setRoutedCount(snapshot->getRoutedCount());
		setIgnoredDisabledCount(snapshot->getIgnoredDisabledCount());
		setEventQueueDroppedCount(snapshot->getEventQueueDroppedCount());
	}

	void LaneCompletionRouter::AdvanceRoutingEpoch()
	{
//		unchecked
			setRoutingEpoch(getRoutingEpoch() + 1);
			if (getRoutingEpoch() == 0)
			{
				setRoutingEpoch(1);
			}
// C# TO C++ CONVERTER TASK: End of the original C# 'unchecked' block.
	}
}
