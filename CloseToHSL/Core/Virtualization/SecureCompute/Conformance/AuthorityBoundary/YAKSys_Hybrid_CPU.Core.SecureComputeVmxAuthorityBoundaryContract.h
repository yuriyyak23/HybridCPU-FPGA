#pragma once

#include "SecureComputeVmxAuthorityBoundaryContract.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class SecureComputeVmxAuthorityBoundaryContract final : public std::enable_shared_from_this<SecureComputeVmxAuthorityBoundaryContract>
	{
	public:
		SecureComputeAuthorityBoundaryViolation Validate(bool vmxActivatesSecureCompute, bool vmxCapsGrantsSecureCompute, bool vmcsStoresSecureState, bool activeVmcsPointerIsDomainIdentity);
	};
}
