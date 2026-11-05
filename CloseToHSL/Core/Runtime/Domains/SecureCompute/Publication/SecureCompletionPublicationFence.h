#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class SecureCompletionFenceState : unsigned char
	{
		Missing = 0,
		Pending = 1,
		CompletionAllowed = 2,
		RetireAllowed = 3
	};

	enum class SecureRetirePublicationRule : unsigned char
	{
		Denied = 0,
		CompletionFenceRequired = 1,
		ExplicitRetireFenceRequired = 2
	};


}
