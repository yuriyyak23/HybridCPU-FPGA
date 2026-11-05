#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class SecureComputeNoEmissionViolation : unsigned char
	{
		None = 0,
		NewInstructionEncoding = 1,
		NewOperandFormat = 2,
		CapabilityAwareLoadStoreFetch = 3,
		VmxSecureModeEmission = 4
	};


}
