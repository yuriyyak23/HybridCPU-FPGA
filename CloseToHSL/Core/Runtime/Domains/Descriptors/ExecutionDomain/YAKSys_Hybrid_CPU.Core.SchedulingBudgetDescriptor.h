#pragma once

#include "SchedulingBudgetDescriptor.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class SchedulingBudgetDescriptor final : public std::enable_shared_from_this<SchedulingBudgetDescriptor>
	{
	private:
		const SchedulingBudgetAuthority Authority;
		const unsigned long long MaxOperationsPerEpoch;
		const bool RequiresSystemSingletonLane;
		const unsigned char PinnedLaneId;

	public:
		SchedulingBudgetDescriptor();

		SchedulingBudgetDescriptor(SchedulingBudgetAuthority authority, unsigned long long maxOperationsPerEpoch, bool requiresSystemSingletonLane, unsigned char pinnedLaneId);

		SchedulingBudgetAuthority getAuthority() const;

		unsigned long long getMaxOperationsPerEpoch() const;

		bool getRequiresSystemSingletonLane() const;

		unsigned char getPinnedLaneId() const;

		bool isRuntimeAuthoritative() const;

		bool isUnbounded() const;

		bool getHasFiniteBudget() const;

		bool AcceptsLane(unsigned char laneId);

		std::shared_ptr<SchedulingBudgetDescriptor> WithMaxOperationsPerEpoch(unsigned long long maxOperationsPerEpoch);
	};
}
