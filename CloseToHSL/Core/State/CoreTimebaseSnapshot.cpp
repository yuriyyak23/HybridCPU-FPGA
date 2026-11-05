#include "CoreTimebaseSnapshot.h"

namespace YAKSys_Hybrid_CPU::Core
{

	CoreTimebaseSnapshot::CoreTimebaseSnapshot(unsigned long long cycleCount, bool isStalled, bool isAvailable, const std::wstring &unavailableReason)
		: CycleCount(cycleCount), IsStalled(isStalled), IsAvailable(isAvailable),
		  UnavailableReason(unavailableReason.empty() ? L"" : unavailableReason)
	{
	}

	unsigned long long CoreTimebaseSnapshot::getCycleCount() const
	{
		return CycleCount;
	}

	bool CoreTimebaseSnapshot::isStalled() const
	{
		return IsStalled;
	}

	bool CoreTimebaseSnapshot::isAvailable() const
	{
		return IsAvailable;
	}

	std::wstring CoreTimebaseSnapshot::getUnavailableReason() const
	{
		return UnavailableReason;
	}
}
