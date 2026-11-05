#include "YAKSys_Hybrid_CPU.Core.SchedulingBudgetTimer.h"
#include "NeutralTrapResult.h"
#include "TrapRequest.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	bool SchedulingBudgetTimer::getArmed() const
	{
		return Armed;
	}

	void SchedulingBudgetTimer::setArmed(bool value)
	{
		Armed = value;
	}

	unsigned long long SchedulingBudgetTimer::getDeadlineCycle() const
	{
		return DeadlineCycle;
	}

	void SchedulingBudgetTimer::setDeadlineCycle(unsigned long long value)
	{
		DeadlineCycle = value;
	}

	unsigned long long SchedulingBudgetTimer::getTimerEpoch() const
	{
		return TimerEpoch;
	}

	void SchedulingBudgetTimer::setTimerEpoch(unsigned long long value)
	{
		TimerEpoch = value;
	}

	unsigned char SchedulingBudgetTimer::getTargetVtId() const
	{
		return TargetVtId;
	}

	void SchedulingBudgetTimer::setTargetVtId(unsigned char value)
	{
		TargetVtId = value;
	}

	unsigned short SchedulingBudgetTimer::getExecutionDomainTag() const
	{
		return ExecutionDomainTag;
	}

	void SchedulingBudgetTimer::setExecutionDomainTag(unsigned short value)
	{
		ExecutionDomainTag = value;
	}

	unsigned short SchedulingBudgetTimer::getAddressSpaceTag() const
	{
		return AddressSpaceTag;
	}

	void SchedulingBudgetTimer::setAddressSpaceTag(unsigned short value)
	{
		AddressSpaceTag = value;
	}

	bool SchedulingBudgetTimer::getLane7PressurePreemptionArmed() const
	{
		return Lane7PressurePreemptionArmed;
	}

	void SchedulingBudgetTimer::setLane7PressurePreemptionArmed(bool value)
	{
		Lane7PressurePreemptionArmed = value;
	}

	unsigned long long SchedulingBudgetTimer::getLane7PressureEpoch() const
	{
		return Lane7PressureEpoch;
	}

	void SchedulingBudgetTimer::setLane7PressureEpoch(unsigned long long value)
	{
		Lane7PressureEpoch = value;
	}

	void SchedulingBudgetTimer::Arm(unsigned long long nowCycle, unsigned long long deltaCycles, unsigned char targetVtId, unsigned short executionDomainTag, unsigned short addressSpaceTag)
	{
		setArmed(true);
		setDeadlineCycle(nowCycle + deltaCycles);
		setTargetVtId(targetVtId);
		setExecutionDomainTag(executionDomainTag);
		setAddressSpaceTag(addressSpaceTag);
		setLane7PressurePreemptionArmed(false);
		AdvanceEpoch();
	}

	void SchedulingBudgetTimer::ArmForLane7Pressure(unsigned long long nowCycle, unsigned long long deltaCycles, std::shared_ptr<Lane7PressureSnapshot> pressure)
	{
		setArmed(true);
		setLane7PressurePreemptionArmed(true);
		setDeadlineCycle(nowCycle + deltaCycles);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: TargetVtId = checked((byte)pressure.OwnerVirtualThreadId);
		setTargetVtId(static_cast<unsigned char>(pressure->OwnerVirtualThreadId));
		setExecutionDomainTag(pressure->ExecutionDomainTag);
		setAddressSpaceTag(pressure->AddressSpaceTag);
		setLane7PressureEpoch(pressure->PressureEpoch);
		AdvanceEpoch();
	}

	bool SchedulingBudgetTimer::TryConsumeExpired(unsigned long long nowCycle, std::shared_ptr<NeutralTrapResult> &result)
	{
		if (!getArmed() || nowCycle < getDeadlineCycle())
		{
			result = NeutralTrapResult::Continue(TrapRequest::ForPreemptionTimer(getTargetVtId(), getExecutionDomainTag(), getAddressSpaceTag(), getDeadlineCycle()));
			return false;
		}

		setArmed(false);
		setLane7PressurePreemptionArmed(false);
		AdvanceEpoch();
		std::shared_ptr<TrapRequest> request = TrapRequest::ForPreemptionTimer(getTargetVtId(), getExecutionDomainTag(), getAddressSpaceTag(), getDeadlineCycle());
		result = NeutralTrapResult::Trap(request, NeutralTrapResultKind::PreemptionTimerExpired);
		return true;
	}

	void SchedulingBudgetTimer::AdvanceEpoch()
	{
//		unchecked
			setTimerEpoch(getTimerEpoch() + 1);
// C# TO C++ CONVERTER TASK: End of the original C# 'unchecked' block.
	}

	std::shared_ptr<SchedulingBudgetTimerSnapshot> SchedulingBudgetTimer::CreateSnapshot()
	{
		return std::make_shared<SchedulingBudgetTimerSnapshot>(getArmed(), getDeadlineCycle(), getTimerEpoch(), getTargetVtId(), getExecutionDomainTag(), getAddressSpaceTag(), getLane7PressurePreemptionArmed(), getLane7PressureEpoch());
	}

	void SchedulingBudgetTimer::RestoreSnapshot(std::shared_ptr<SchedulingBudgetTimerSnapshot> snapshot)
	{
		setArmed(snapshot->Armed);
		setDeadlineCycle(snapshot->DeadlineCycle);
		setTimerEpoch(snapshot->TimerEpoch);
		setTargetVtId(snapshot->TargetVtId);
		setExecutionDomainTag(snapshot->ExecutionDomainTag);
		setAddressSpaceTag(snapshot->AddressSpaceTag);
		setLane7PressurePreemptionArmed(snapshot->Lane7PressurePreemptionArmed);
		setLane7PressureEpoch(snapshot->Lane7PressureEpoch);
	}
}
