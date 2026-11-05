#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class SecureComputeEvidenceLeakViolation : unsigned char
	{
		None = 0,
		HostEvidenceGuestVisible = 1,
		SchedulerEvidenceSerialized = 2,
		BackendBindingEvidenceSerialized = 3,
		NativeTokenEvidenceSerialized = 4,
		DebugTraceGuestState = 5
	};


}
