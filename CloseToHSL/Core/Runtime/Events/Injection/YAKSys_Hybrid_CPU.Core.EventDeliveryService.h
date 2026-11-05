#pragma once
#include "EventDeliveryService.h"

#include "YAKSys_Hybrid_CPU.Core.PostedEventQueue.h"
#include "../../Domains/Descriptors/EventQueue/YAKSys_Hybrid_CPU.Core.EventQueueDescriptor.h"
#include "InterruptRemapPolicy.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class EventDeliveryService final : public std::enable_shared_from_this<EventDeliveryService>
	{
	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public EventDeliveryResult Post(EventQueueDescriptor? descriptor, PostedEventQueue queue, EventInjectionDescriptor evt, System.Nullable<InterruptRemapTable> remapTable = null)
		std::shared_ptr<EventDeliveryResult> Post(std::shared_ptr<EventQueueDescriptor> descriptor, std::shared_ptr<PostedEventQueue> queue, std::shared_ptr<EventInjectionDescriptor> evt, std::shared_ptr<InterruptRemapTable> remapTable = nullptr);
	};
}
