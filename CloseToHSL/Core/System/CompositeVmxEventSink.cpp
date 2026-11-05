#include "CompositeVmxEventSink.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<IVmxEventSink> CompositeVmxEventSink::getPrimary() const
	{
		return Primary;
	}

	std::shared_ptr<IVmxEventSink> CompositeVmxEventSink::getSecondary() const
	{
		return Secondary;
	}

	CompositeVmxEventSink::CompositeVmxEventSink(std::shared_ptr<IVmxEventSink> primary, std::shared_ptr<IVmxEventSink> secondary)
		: Primary(primary ? std::move(primary) : throw std::invalid_argument("primary")),
		  Secondary(secondary ? std::move(secondary) : throw std::invalid_argument("secondary"))
	{
	}

	void CompositeVmxEventSink::RecordVmxEvent(VmxEventKind kind, unsigned short vtId, VmExitReason exitReason)
	{
		getPrimary()->RecordVmxEvent(kind, vtId, exitReason);
		getSecondary()->RecordVmxEvent(kind, vtId, exitReason);
	}
}
