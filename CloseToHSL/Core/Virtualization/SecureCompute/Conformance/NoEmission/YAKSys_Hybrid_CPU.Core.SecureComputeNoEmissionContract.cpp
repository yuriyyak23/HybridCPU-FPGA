#include "YAKSys_Hybrid_CPU.Core.SecureComputeNoEmissionContract.h"

namespace YAKSys_Hybrid_CPU::Core
{

	SecureComputeNoEmissionViolation SecureComputeNoEmissionContract::Validate(bool emitsNewInstructionEncoding, bool emitsNewOperandFormat, bool emitsCapabilityAwareLoadStoreFetch, bool emitsVmxSecureMode)
	{
		if (emitsNewInstructionEncoding)
		{
			return SecureComputeNoEmissionViolation::NewInstructionEncoding;
		}

		if (emitsNewOperandFormat)
		{
			return SecureComputeNoEmissionViolation::NewOperandFormat;
		}

		if (emitsCapabilityAwareLoadStoreFetch)
		{
			return SecureComputeNoEmissionViolation::CapabilityAwareLoadStoreFetch;
		}

		if (emitsVmxSecureMode)
		{
			return SecureComputeNoEmissionViolation::VmxSecureModeEmission;
		}

		return SecureComputeNoEmissionViolation::None;
	}
}
