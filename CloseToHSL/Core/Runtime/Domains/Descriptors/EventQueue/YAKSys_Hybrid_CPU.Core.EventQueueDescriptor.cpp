#include "YAKSys_Hybrid_CPU.Core.EventQueueDescriptor.h"

namespace YAKSys_Hybrid_CPU::Core
{

	EventQueueDescriptor::EventQueueDescriptor() : EventQueueDescriptor(EventQueueAuthority::Runtime, DefaultMaxPendingEvents, true, true, true)
	{
	}

	EventQueueDescriptor::EventQueueDescriptor(EventQueueAuthority authority, int maxPendingEvents, bool prefersPriorityDelivery, bool allowsEventCoalescing, bool allowsCompatibilityProjection)
		: Authority(authority),
		  MaxPendingEvents(maxPendingEvents <= 0 ? 1 : maxPendingEvents),
		  PrefersPriorityDelivery(prefersPriorityDelivery),
		  AllowsEventCoalescing(allowsEventCoalescing),
		  AllowsCompatibilityProjection(allowsCompatibilityProjection)
	{
	}

	EventQueueAuthority EventQueueDescriptor::getAuthority() const
	{
		return Authority;
	}

	int EventQueueDescriptor::getMaxPendingEvents() const
	{
		return MaxPendingEvents;
	}

	bool EventQueueDescriptor::getPrefersPriorityDelivery() const
	{
		return PrefersPriorityDelivery;
	}

	bool EventQueueDescriptor::getAllowsEventCoalescing() const
	{
		return AllowsEventCoalescing;
	}

	bool EventQueueDescriptor::getAllowsCompatibilityProjection() const
	{
		return AllowsCompatibilityProjection;
	}

	bool EventQueueDescriptor::isRuntimeAuthoritative() const
	{
		return getAuthority() == EventQueueAuthority::Runtime;
	}

	bool EventQueueDescriptor::getCanProjectToCompatibilityFrontend() const
	{
		return getAllowsCompatibilityProjection() && isRuntimeAuthoritative();
	}

	bool EventQueueDescriptor::AcceptsPendingCount(int pendingCount)
	{
		return pendingCount >= 0 && pendingCount < getMaxPendingEvents();
	}

	std::shared_ptr<EventQueueDescriptor> EventQueueDescriptor::WithMaxPendingEvents(int maxPendingEvents)
	{
		return std::make_shared<EventQueueDescriptor>(getAuthority(), maxPendingEvents, getPrefersPriorityDelivery(), getAllowsEventCoalescing(), getAllowsCompatibilityProjection());
	}
}
