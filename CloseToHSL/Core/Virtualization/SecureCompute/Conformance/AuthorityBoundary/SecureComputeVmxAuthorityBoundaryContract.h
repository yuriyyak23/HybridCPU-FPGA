#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class SecureComputeAuthorityBoundaryViolation : unsigned char
	{
		None = 0,
		VmxActivation = 1,
		VmxCapsAuthority = 2,
		VmcsStateStore = 3,
		ActiveVmcsPointerIdentity = 4
	};


}
