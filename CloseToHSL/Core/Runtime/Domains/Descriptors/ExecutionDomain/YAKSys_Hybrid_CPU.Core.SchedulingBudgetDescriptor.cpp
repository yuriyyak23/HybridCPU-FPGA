#include "YAKSys_Hybrid_CPU.Core.SchedulingBudgetDescriptor.h"
#include "../../../../Virtualization/CompilerBoundary/LaneBinding/YAKSys_Hybrid_CPU.Core.VirtualizationLaneBindingPolicy.h"

namespace YAKSys_Hybrid_CPU::Core
{

	SchedulingBudgetDescriptor::SchedulingBudgetDescriptor() : SchedulingBudgetDescriptor(SchedulingBudgetAuthority::Runtime, 0, true, VirtualizationLaneBindingPolicy::Lane7Id)
	{
	}

	SchedulingBudgetDescriptor::SchedulingBudgetDescriptor(SchedulingBudgetAuthority authority, unsigned long long maxOperationsPerEpoch, bool requiresSystemSingletonLane, unsigned char pinnedLaneId)
		: Authority(authority),
		  MaxOperationsPerEpoch(maxOperationsPerEpoch),
		  RequiresSystemSingletonLane(requiresSystemSingletonLane),
		  PinnedLaneId(pinnedLaneId)
	{
	}

	SchedulingBudgetAuthority SchedulingBudgetDescriptor::getAuthority() const
	{
		return Authority;
	}

	unsigned long long SchedulingBudgetDescriptor::getMaxOperationsPerEpoch() const
	{
		return MaxOperationsPerEpoch;
	}

	bool SchedulingBudgetDescriptor::getRequiresSystemSingletonLane() const
	{
		return RequiresSystemSingletonLane;
	}

	unsigned char SchedulingBudgetDescriptor::getPinnedLaneId() const
	{
		return PinnedLaneId;
	}

	bool SchedulingBudgetDescriptor::isRuntimeAuthoritative() const
	{
		return getAuthority() == SchedulingBudgetAuthority::Runtime;
	}

	bool SchedulingBudgetDescriptor::isUnbounded() const
	{
		return getMaxOperationsPerEpoch() == 0;
	}

	bool SchedulingBudgetDescriptor::getHasFiniteBudget() const
	{
		return getMaxOperationsPerEpoch() != 0;
	}

	bool SchedulingBudgetDescriptor::AcceptsLane(unsigned char laneId)
	{
		return !getRequiresSystemSingletonLane() || laneId == getPinnedLaneId();
	}

	std::shared_ptr<SchedulingBudgetDescriptor> SchedulingBudgetDescriptor::WithMaxOperationsPerEpoch(unsigned long long maxOperationsPerEpoch)
	{
		return std::make_shared<SchedulingBudgetDescriptor>(
			getAuthority(), maxOperationsPerEpoch, getRequiresSystemSingletonLane(), getPinnedLaneId());
	}
}


