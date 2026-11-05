#include "YAKSys_Hybrid_CPU.Core.SecureComputeVmxAuthorityBoundaryContract.h"

namespace YAKSys_Hybrid_CPU::Core
{

	SecureComputeAuthorityBoundaryViolation SecureComputeVmxAuthorityBoundaryContract::Validate(bool vmxActivatesSecureCompute, bool vmxCapsGrantsSecureCompute, bool vmcsStoresSecureState, bool activeVmcsPointerIsDomainIdentity)
	{
		if (vmxActivatesSecureCompute)
		{
			return SecureComputeAuthorityBoundaryViolation::VmxActivation;
		}

		if (vmxCapsGrantsSecureCompute)
		{
			return SecureComputeAuthorityBoundaryViolation::VmxCapsAuthority;
		}

		if (vmcsStoresSecureState)
		{
			return SecureComputeAuthorityBoundaryViolation::VmcsStateStore;
		}

		if (activeVmcsPointerIsDomainIdentity)
		{
			return SecureComputeAuthorityBoundaryViolation::ActiveVmcsPointerIdentity;
		}

		return SecureComputeAuthorityBoundaryViolation::None;
	}
}
