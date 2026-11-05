#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class SecureComputeVmReadVisibilityDecision : unsigned char
	{
		AllowedReadOnlyProjection = 0,
		DeniedNoNeutralOwner = 1,
		DeniedNoReadOnlySource = 2,
		DeniedNoSecureVisibility = 3,
		DeniedNoMigrationClass = 4,
		DeniedNoConformanceProof = 5
	};


}
