#pragma once

#include "YAKSys_Hybrid_CPU.Core.PostedEventQueue.h"
#include "InterruptRemapPolicy.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class VirtualInterruptFabric final : public std::enable_shared_from_this<VirtualInterruptFabric>
	{
	private:
		unsigned long long RemapDroppedCount = 0;
		unsigned long long RemappedCount = 0;
		unsigned long long CoalescedCount = 0;
		unsigned long long RoutingEpoch = 0;
		unsigned long long DeliveredCount = 0;
		unsigned long long EoiCount = 0;

		const std::shared_ptr<PostedEventQueue> _pending;
		const std::shared_ptr<InterruptRemapTable> _remap = std::make_shared<InterruptRemapTable>();
		unsigned long long _inServiceBits = 0;

	public:
		VirtualInterruptFabric(int capacity = 64);

		int getPendingCount() const;

		unsigned long long getDroppedCount() const;

		unsigned long long getInvalidDroppedCount() const;

		unsigned long long getCapacityDroppedCount() const;

		unsigned long long getRemapDroppedCount() const;
	private:
		void setRemapDroppedCount(unsigned long long value);

	public:
		unsigned long long getRemappedCount() const;
	private:
		void setRemappedCount(unsigned long long value);

	public:
		unsigned long long getCoalescedCount() const;
	private:
		void setCoalescedCount(unsigned long long value);

	public:
		unsigned long long getRoutingEpoch() const;
	private:
		void setRoutingEpoch(unsigned long long value);

	public:
		unsigned long long getRemapPolicyEpoch() const;

		unsigned long long getDeliveredCount() const;
	private:
		void setDeliveredCount(unsigned long long value);

	public:
		unsigned long long getEoiCount() const;
	private:
		void setEoiCount(unsigned long long value);

	public:
		bool TryPost(std::shared_ptr<EventInjectionDescriptor> descriptor, std::shared_ptr<EventInjectionDescriptor> &queued);

		bool TryPost(std::shared_ptr<EventInjectionDescriptor> descriptor, std::shared_ptr<EventInjectionDescriptor> &queued, InterruptPostDisposition &disposition);

		bool TryDeliver(unsigned char targetVtId, unsigned short executionDomainTag, bool blocked, std::shared_ptr<EventInjectionDescriptor> &descriptor);

		void CompleteEoi(unsigned char vector);

		bool IsInService(unsigned char vector);

	private:
		void AdvanceRoutingEpoch();


	public:
		void ConfigureRemap(std::shared_ptr<InterruptRemapEntry> entry);

		bool RemoveRemap(std::shared_ptr<InterruptRemapKey> key);

		void ClearRemaps();
	};
}
