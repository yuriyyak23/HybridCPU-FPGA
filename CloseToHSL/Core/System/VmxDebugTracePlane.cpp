#include "VmxDebugTracePlane.h"

namespace YAKSys_Hybrid_CPU::Core
{

	void VmxDebugTracePlane::RecordVmxEvent(VmxEventKind kind, unsigned short vtId, VmExitReason exitReason)
	{
		RecordEvent(kind, exitReason);
	}

	void VmxDebugTracePlane::RecordEvent(VmxEventKind kind, VmExitReason exitReason)
	{
		setVmxEventCount(getVmxEventCount() + 1);
		if (kind == VmxEventKind::VmExit)
		{
			RecordExit(exitReason);
		}
	}

	unsigned long long VmxDebugTracePlane::getVmxEventCount() const
	{
		return VmxEventCount;
	}

	void VmxDebugTracePlane::setVmxEventCount(unsigned long long value)
	{
		VmxEventCount = value;
	}

	unsigned long long VmxDebugTracePlane::getVmExitCount() const
	{
		return VmExitCount;
	}

	void VmxDebugTracePlane::setVmExitCount(unsigned long long value)
	{
		VmExitCount = value;
	}

	unsigned long long VmxDebugTracePlane::getVmFailCount() const
	{
		return VmFailCount;
	}

	void VmxDebugTracePlane::setVmFailCount(unsigned long long value)
	{
		VmFailCount = value;
	}

	unsigned long long VmxDebugTracePlane::getVmAbortCount() const
	{
		return VmAbortCount;
	}

	void VmxDebugTracePlane::setVmAbortCount(unsigned long long value)
	{
		VmAbortCount = value;
	}

	unsigned long long VmxDebugTracePlane::getInvalidationCount() const
	{
		return InvalidationCount;
	}

	void VmxDebugTracePlane::setInvalidationCount(unsigned long long value)
	{
		InvalidationCount = value;
	}

	unsigned long long VmxDebugTracePlane::getDroppedPostedEventCount() const
	{
		return DroppedPostedEventCount;
	}

	void VmxDebugTracePlane::setDroppedPostedEventCount(unsigned long long value)
	{
		DroppedPostedEventCount = value;
	}

	VmExitReason VmxDebugTracePlane::getLastExitReason() const
	{
		return LastExitReason;
	}

	void VmxDebugTracePlane::setLastExitReason(VmExitReason value)
	{
		LastExitReason = value;
	}

	void VmxDebugTracePlane::RecordExit(VmExitReason reason)
	{
		setVmExitCount(getVmExitCount() + 1);
		setLastExitReason(reason);
	}

	void VmxDebugTracePlane::RecordFail()
	{
		setVmFailCount(getVmFailCount() + 1);
	}

	void VmxDebugTracePlane::RecordAbort()
	{
		setVmAbortCount(getVmAbortCount() + 1);
	}

	void VmxDebugTracePlane::RecordInvalidation()
	{
		setInvalidationCount(getInvalidationCount() + 1);
	}

	void VmxDebugTracePlane::RecordDroppedPostedEvents(unsigned long long count)
	{
		setDroppedPostedEventCount(getDroppedPostedEventCount() + count);
	}

	std::shared_ptr<VmxDebugTraceCounters> VmxDebugTracePlane::Snapshot()
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return new(VmxEventCount, VmExitCount, VmFailCount, VmAbortCount, InvalidationCount, DroppedPostedEventCount, unchecked((ulong)LastExitReason));
		return std::make_shared<VmxDebugTraceCounters>(getVmxEventCount(), getVmExitCount(), getVmFailCount(),
			getVmAbortCount(), getInvalidationCount(), getDroppedPostedEventCount(),
			static_cast<unsigned long long>(getLastExitReason()));
	}

	void VmxDebugTracePlane::Reset()
	{
		setVmxEventCount(0);
		setVmExitCount(0);
		setVmFailCount(0);
		setVmAbortCount(0);
		setInvalidationCount(0);
		setDroppedPostedEventCount(0);
		setLastExitReason(VmExitReason::None);
	}

	void VmxDebugTracePlane::DiscardTraceHandles()
	{
		Reset();
	}
}
