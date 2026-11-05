#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class SecureComputeVmWriteDecision : unsigned char
	{
		DeniedSecureStateMutation = 0,
		DeniedProjectionOnlyField = 1
	};


}
