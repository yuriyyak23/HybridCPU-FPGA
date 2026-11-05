#include "YAKSys_Hybrid_CPU.Core.VirtualInterruptFabric.h"

namespace YAKSys_Hybrid_CPU::Core
{

	VirtualInterruptFabric::VirtualInterruptFabric(int capacity) : _pending(std::make_shared<PostedEventQueue>(capacity))
	{
	}

	int VirtualInterruptFabric::getPendingCount() const
	{
		return _pending->getCount();
	}

	unsigned long long VirtualInterruptFabric::getDroppedCount() const
	{
		return _pending->getDroppedCount() + getRemapDroppedCount();
	}

	unsigned long long VirtualInterruptFabric::getInvalidDroppedCount() const
	{
		return _pending->getInvalidDroppedCount();
	}

	unsigned long long VirtualInterruptFabric::getCapacityDroppedCount() const
	{
		return _pending->getCapacityDroppedCount();
	}

	unsigned long long VirtualInterruptFabric::getRemapDroppedCount() const
	{
		return RemapDroppedCount;
	}

	void VirtualInterruptFabric::setRemapDroppedCount(unsigned long long value)
	{
		RemapDroppedCount = value;
	}

	unsigned long long VirtualInterruptFabric::getRemappedCount() const
	{
		return RemappedCount;
	}

	void VirtualInterruptFabric::setRemappedCount(unsigned long long value)
	{
		RemappedCount = value;
	}

	unsigned long long VirtualInterruptFabric::getCoalescedCount() const
	{
		return CoalescedCount;
	}

	void VirtualInterruptFabric::setCoalescedCount(unsigned long long value)
	{
		CoalescedCount = value;
	}

	unsigned long long VirtualInterruptFabric::getRoutingEpoch() const
	{
		return RoutingEpoch;
	}

	void VirtualInterruptFabric::setRoutingEpoch(unsigned long long value)
	{
		RoutingEpoch = value;
	}

	unsigned long long VirtualInterruptFabric::getRemapPolicyEpoch() const
	{
		return _remap->getPolicyEpoch();
	}

	unsigned long long VirtualInterruptFabric::getDeliveredCount() const
	{
		return DeliveredCount;
	}

	void VirtualInterruptFabric::setDeliveredCount(unsigned long long value)
	{
		DeliveredCount = value;
	}

	unsigned long long VirtualInterruptFabric::getEoiCount() const
	{
		return EoiCount;
	}

	void VirtualInterruptFabric::setEoiCount(unsigned long long value)
	{
		EoiCount = value;
	}

	bool VirtualInterruptFabric::TryPost(std::shared_ptr<EventInjectionDescriptor> descriptor, std::shared_ptr<EventInjectionDescriptor> &queued)
	{
		InterruptPostDisposition ignored; return TryPost(descriptor, queued, ignored);
	}

	bool VirtualInterruptFabric::TryPost(std::shared_ptr<EventInjectionDescriptor> descriptor, std::shared_ptr<EventInjectionDescriptor> &queued, InterruptPostDisposition &disposition)
	{
		queued = nullptr;

		std::shared_ptr<EventInjectionDescriptor> routed;
		bool remapped;
		bool coalescePosted;
		if (!_remap->TryRoute(descriptor, routed, remapped, coalescePosted, disposition))
		{
			if (disposition == InterruptPostDisposition::DroppedByRemap)
			{
				setRemapDroppedCount(getRemapDroppedCount() + 1);
				AdvanceRoutingEpoch();
				return false;
			}

			std::shared_ptr<EventInjectionDescriptor> ignored; _pending->TryEnqueue(descriptor, ignored, disposition);
			AdvanceRoutingEpoch();
			return false;
		}

		if (remapped)
		{
			setRemappedCount(getRemappedCount() + 1);
		}

// C# TO C++ CONVERTER TASK: C# 'with expressions' are not converted by C# to C++ Converter:
auto posted = std::make_shared<EventInjectionDescriptor>(*routed); posted->Posted = true;
		std::shared_ptr<EventInjectionDescriptor> existing;
		if (coalescePosted && _pending->TryFindCoalescingMatch(posted, existing))
		{
			queued = existing;
			setCoalescedCount(getCoalescedCount() + 1);
			disposition = remapped ? InterruptPostDisposition::RemappedCoalesced : InterruptPostDisposition::Coalesced;
			AdvanceRoutingEpoch();
			return true;
		}

		bool accepted = _pending->TryEnqueue(posted, queued, disposition);
		if (accepted)
		{
			disposition = remapped ? InterruptPostDisposition::RemappedQueued : InterruptPostDisposition::Queued;
		}

		AdvanceRoutingEpoch();
		return accepted;
	}

	bool VirtualInterruptFabric::TryDeliver(unsigned char targetVtId, unsigned short executionDomainTag, bool blocked, std::shared_ptr<EventInjectionDescriptor> &descriptor)
	{
		if (blocked || !_pending->TryDequeueMatching(targetVtId, executionDomainTag, true, descriptor))
		{
			descriptor = nullptr;
			return false;
		}

		if (descriptor->Vector < 64)
		{
			_inServiceBits |= 1ULL << descriptor->Vector;
		}

		setDeliveredCount(getDeliveredCount() + 1);
		AdvanceRoutingEpoch();
		return true;
	}

	void VirtualInterruptFabric::CompleteEoi(unsigned char vector)
	{
		if (vector < 64)
		{
			_inServiceBits &= ~(1ULL << vector);
		}

		setEoiCount(getEoiCount() + 1);
		AdvanceRoutingEpoch();
	}

	bool VirtualInterruptFabric::IsInService(unsigned char vector)
	{
		return vector < 64 && (_inServiceBits & (1ULL << vector)) != 0;
	}

	void VirtualInterruptFabric::AdvanceRoutingEpoch()
	{
//		unchecked
			setRoutingEpoch(getRoutingEpoch() + 1);
// C# TO C++ CONVERTER TASK: End of the original C# 'unchecked' block.
	}

	void VirtualInterruptFabric::ConfigureRemap(std::shared_ptr<InterruptRemapEntry> entry)
	{
		_remap->Configure(entry);
		AdvanceRoutingEpoch();
	}

	bool VirtualInterruptFabric::RemoveRemap(std::shared_ptr<InterruptRemapKey> key)
	{
		bool removed = _remap->Remove(key);
		if (removed)
		{
			AdvanceRoutingEpoch();
		}

		return removed;
	}

	void VirtualInterruptFabric::ClearRemaps()
	{
		unsigned long long before = _remap->getPolicyEpoch();
		_remap->Clear();
		if (_remap->getPolicyEpoch() != before)
		{
			AdvanceRoutingEpoch();
		}
	}
}
