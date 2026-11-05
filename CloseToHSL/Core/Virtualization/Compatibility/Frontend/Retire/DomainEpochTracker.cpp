#include "DomainEpochTracker.h"

namespace YAKSys_Hybrid_CPU::Core
{

	unsigned long long DomainEpochTracker::getCurrentEpoch() const
	{
		return _epoch;
	}

	std::optional<VmxEventKind> DomainEpochTracker::getLastTransitionKind() const
	{
		if (!_hasTransitioned)
		{
			return std::nullopt;
		}
		return std::optional<VmxEventKind>{_lastTransitionKind};
	}

	bool DomainEpochTracker::IsEpochBoundary(VmxEventKind kind)
	{
		return (kind == VmxEventKind::VmEntry || kind == VmxEventKind::VmResume || kind == VmxEventKind::VmExit || kind == VmxEventKind::VmxOn || kind == VmxEventKind::VmxOff);
	}

	void DomainEpochTracker::RecordVmxEvent(VmxEventKind kind, unsigned short vtId, VmExitReason exitReason)
	{
		if (IsEpochBoundary(kind))
		{
			_epoch++;
			_lastTransitionKind = kind;
			_hasTransitioned = true;
		}
	}

	bool DomainEpochTracker::IsCurrentEpoch(unsigned long long cachedEpoch)
	{
		return cachedEpoch == _epoch;
	}

	void DomainEpochTracker::Reset()
	{
		_epoch = 0;
		_lastTransitionKind = VmxEventKind::VmxOff;
		_hasTransitioned = false;
	}
}
