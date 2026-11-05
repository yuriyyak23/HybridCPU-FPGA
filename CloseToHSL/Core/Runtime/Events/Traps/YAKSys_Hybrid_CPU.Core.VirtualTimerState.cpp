#include "YAKSys_Hybrid_CPU.Core.VirtualTimerState.h"
#include "../Injection/EventInjectionDescriptor.h"

namespace YAKSys_Hybrid_CPU::Core
{

	bool VirtualTimerState::getArmed() const
	{
		return Armed;
	}

	void VirtualTimerState::setArmed(bool value)
	{
		Armed = value;
	}

	unsigned long long VirtualTimerState::getDeadlineCycle() const
	{
		return DeadlineCycle;
	}

	void VirtualTimerState::setDeadlineCycle(unsigned long long value)
	{
		DeadlineCycle = value;
	}

	unsigned long long VirtualTimerState::getTimerEpoch() const
	{
		return TimerEpoch;
	}

	void VirtualTimerState::setTimerEpoch(unsigned long long value)
	{
		TimerEpoch = value;
	}

	unsigned char VirtualTimerState::getTargetVtId() const
	{
		return TargetVtId;
	}

	void VirtualTimerState::setTargetVtId(unsigned char value)
	{
		TargetVtId = value;
	}

	unsigned short VirtualTimerState::getExecutionDomainTag() const
	{
		return ExecutionDomainTag;
	}

	void VirtualTimerState::setExecutionDomainTag(unsigned short value)
	{
		ExecutionDomainTag = value;
	}

	unsigned short VirtualTimerState::getAddressSpaceTag() const
	{
		return AddressSpaceTag;
	}

	void VirtualTimerState::setAddressSpaceTag(unsigned short value)
	{
		AddressSpaceTag = value;
	}

	unsigned char VirtualTimerState::getVector() const
	{
		return Vector;
	}

	void VirtualTimerState::setVector(unsigned char value)
	{
		Vector = value;
	}

	unsigned char VirtualTimerState::getPriority() const
	{
		return Priority;
	}

	void VirtualTimerState::setPriority(unsigned char value)
	{
		Priority = value;
	}

	void VirtualTimerState::Arm(unsigned long long nowCycle, unsigned long long deltaCycles, unsigned char targetVtId, unsigned short executionDomainTag, unsigned short addressSpaceTag, unsigned char vector, unsigned char priority)
	{
		setArmed(true);
		setDeadlineCycle(nowCycle + deltaCycles);
		setTargetVtId(targetVtId);
		setExecutionDomainTag(executionDomainTag);
		setAddressSpaceTag(addressSpaceTag);
		setVector(vector);
		setPriority(priority);
		AdvanceEpoch();
	}

	void VirtualTimerState::AdvanceEpoch()
	{
//		unchecked
			setTimerEpoch(getTimerEpoch() + 1);
// C# TO C++ CONVERTER TASK: End of the original C# 'unchecked' block.
	}

	bool VirtualTimerState::TryConsumeExpired(unsigned long long nowCycle, std::shared_ptr<EventInjectionDescriptor> &descriptor)
	{
		if (!getArmed() || nowCycle < getDeadlineCycle())
		{
			descriptor = nullptr;
			return false;
		}

		setArmed(false);
		AdvanceEpoch();
		descriptor = EventInjectionDescriptor::Create(EventInjectionKind::VirtualTimer, getVector(), getTargetVtId(), getExecutionDomainTag(), getAddressSpaceTag(), getPriority(), getDeadlineCycle(), false);
		return true;
	}
}
