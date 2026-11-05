#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class EvidenceVisibilityClass : unsigned char
	{
		GuestArchitecturalState = 0,
		CompatibilityAlias = 1,
		HostOwnedRuntimeEvidence = 2,
		SchedulerEvidence = 3,
		BackendBindingEvidence = 4,
		NativeTokenEvidence = 5
	};

	enum class EvidenceRestorePolicy : unsigned char
	{
		RecomputeAfterRestore = 0,
		ZeroizeAfterRestore = 1,
		PreserveGuestArchitecturalState = 2
	};


}
