#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class HostEvidenceNonLeakViolation : unsigned char
	{
		None = 0,
		EvidenceEnvelopeLeak = 1,
		CompletionEnvelopeLeak = 2,
		DescriptorEnvelopeLeak = 3,
		GuestVisibleProjectionLeak = 4
	};


}
