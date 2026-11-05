#pragma once

#include "EventQueueDescriptor.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class EventQueueDescriptor final : public std::enable_shared_from_this<EventQueueDescriptor>
	{
	private:
		const EventQueueAuthority Authority;
		const int MaxPendingEvents;
		const bool PrefersPriorityDelivery;
		const bool AllowsEventCoalescing;
		const bool AllowsCompatibilityProjection;

	public:
		static constexpr int DefaultMaxPendingEvents = 64;

		EventQueueDescriptor();

		EventQueueDescriptor(EventQueueAuthority authority, int maxPendingEvents, bool prefersPriorityDelivery, bool allowsEventCoalescing, bool allowsCompatibilityProjection);

		EventQueueAuthority getAuthority() const;

		int getMaxPendingEvents() const;

		bool getPrefersPriorityDelivery() const;

		bool getAllowsEventCoalescing() const;

		bool getAllowsCompatibilityProjection() const;

		bool isRuntimeAuthoritative() const;

		bool getCanProjectToCompatibilityFrontend() const;

		bool AcceptsPendingCount(int pendingCount);

		std::shared_ptr<EventQueueDescriptor> WithMaxPendingEvents(int maxPendingEvents);
	};
}
