#include "YAKSys_Hybrid_CPU.Core.EventDeliveryService.h"
#include "../../Domains/Validation/DomainValidationResult.h"
#include "../../Domains/Validation/YAKSys_Hybrid_CPU.Core.DomainValidationResult.h"

namespace YAKSys_Hybrid_CPU::Core
{

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public EventDeliveryResult Post(EventQueueDescriptor? descriptor, PostedEventQueue queue, EventInjectionDescriptor evt, System.Nullable<InterruptRemapTable> remapTable = null)
	std::shared_ptr<EventDeliveryResult> EventDeliveryService::Post(std::shared_ptr<EventQueueDescriptor> descriptor, std::shared_ptr<PostedEventQueue> queue, std::shared_ptr<EventInjectionDescriptor> evt, std::shared_ptr<InterruptRemapTable> remapTable)
	{
		if (descriptor == nullptr)
		{
			return EventDeliveryResult::Fail(DomainValidationFailureReason::MissingEventQueueDescriptor, evt, InterruptPostDisposition::DroppedInvalid, L"Event delivery requires a runtime-owned event queue descriptor.");
		}

		if (!descriptor->isRuntimeAuthoritative())
		{
			return EventDeliveryResult::Fail(DomainValidationFailureReason::RuntimeAuthorityMissing, evt, InterruptPostDisposition::DroppedInvalid, L"Compatibility projection cannot own event delivery.");
		}

		if (!evt || !evt->IsValid())
		{
			return EventDeliveryResult::Fail(DomainValidationFailureReason::EventQueueRejected, evt, InterruptPostDisposition::DroppedInvalid, L"Event delivery rejected an invalid event descriptor.");
		}

		std::shared_ptr<EventInjectionDescriptor> routed = evt;
		bool remapped = false;
		bool coalescePosted = descriptor->getAllowsEventCoalescing() && evt->Posted;
		bool remapCoalesces = true;
		InterruptPostDisposition disposition = InterruptPostDisposition::None;
		if (remapTable != nullptr && !remapTable->TryRoute(evt, routed, remapped, remapCoalesces, disposition))
		{
			return EventDeliveryResult::Fail(DomainValidationFailureReason::EventQueueRejected, routed, disposition, L"Event delivery was dropped by the runtime remap policy.");
		}

		coalescePosted &= remapTable == nullptr || remapCoalesces;
		if (!descriptor->AcceptsPendingCount(queue->getCount()))
		{
			return EventDeliveryResult::Fail(DomainValidationFailureReason::EventQueueRejected, routed, InterruptPostDisposition::DroppedQueueFull, L"Event queue descriptor denies additional pending events.");
		}

		std::shared_ptr<EventInjectionDescriptor> existing;
		if (coalescePosted && queue->TryFindCoalescingMatch(routed, existing))
		{
			return std::make_shared<EventDeliveryResult>(DomainValidationResult::getPassed(), existing, remapped ? InterruptPostDisposition::RemappedCoalesced : InterruptPostDisposition::Coalesced);
		}

		std::shared_ptr<EventInjectionDescriptor> queued;
		if (!queue->TryEnqueue(routed, queued, disposition))
		{
			return EventDeliveryResult::Fail(DomainValidationFailureReason::EventQueueRejected, routed, disposition, L"Posted event queue rejected the event descriptor.");
		}

		return std::make_shared<EventDeliveryResult>(DomainValidationResult::getPassed(), queued, remapped ? InterruptPostDisposition::RemappedQueued : InterruptPostDisposition::Queued);
	}
}
