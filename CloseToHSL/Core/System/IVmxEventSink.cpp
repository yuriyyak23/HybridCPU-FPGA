#include "IVmxEventSink.h"

namespace YAKSys_Hybrid_CPU::Core
{

	void NullVmxEventSink::RecordVmxEvent(VmxEventKind kind, unsigned short vtId, VmExitReason exitReason)
	{
		// Intentionally empty.
	}
}
