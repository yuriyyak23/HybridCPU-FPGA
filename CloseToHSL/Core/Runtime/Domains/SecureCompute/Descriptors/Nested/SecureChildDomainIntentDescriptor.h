#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class SecureChildDomainIntentState : unsigned char
	{
		Missing = 0,
		Declared = 1,
		ParentPolicyValidated = 2,
		Denied = 3
	};


}
