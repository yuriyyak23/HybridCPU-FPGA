#include "YAKSys_Hybrid_CPU.Core.SecureComputeVmWriteDenyPolicy.h"

namespace YAKSys_Hybrid_CPU::Core
{

	SecureComputeVmWriteDecision SecureComputeVmWriteDenyPolicy::Deny(bool secureSensitiveField)
	{
		return secureSensitiveField ? SecureComputeVmWriteDecision::DeniedSecureStateMutation : SecureComputeVmWriteDecision::DeniedProjectionOnlyField;
	}
}
