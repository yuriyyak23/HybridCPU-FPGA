#include "IV4TraceEventSink.h"

namespace YAKSys_Hybrid_CPU::Core
{

	NullV4TraceEventSink::NullV4TraceEventSink()
	{
	}

	void NullV4TraceEventSink::RecordV4Event(V4TraceEvent evt)
	{
		// No-op — events are discarded
	}
}
