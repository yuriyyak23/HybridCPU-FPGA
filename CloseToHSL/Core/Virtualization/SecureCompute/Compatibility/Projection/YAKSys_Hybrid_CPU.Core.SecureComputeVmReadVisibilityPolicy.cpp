#include "YAKSys_Hybrid_CPU.Core.SecureComputeVmReadVisibilityPolicy.h"

namespace YAKSys_Hybrid_CPU::Core
{

	SecureComputeVmReadVisibilityDecision SecureComputeVmReadVisibilityPolicy::Validate(bool hasNeutralOwner, bool hasReadOnlySource, bool hasSecureVisibility, bool hasMigrationClassification, bool hasConformanceProof)
	{
		if (!hasNeutralOwner)
		{
			return SecureComputeVmReadVisibilityDecision::DeniedNoNeutralOwner;
		}

		if (!hasReadOnlySource)
		{
			return SecureComputeVmReadVisibilityDecision::DeniedNoReadOnlySource;
		}

		if (!hasSecureVisibility)
		{
			return SecureComputeVmReadVisibilityDecision::DeniedNoSecureVisibility;
		}

		if (!hasMigrationClassification)
		{
			return SecureComputeVmReadVisibilityDecision::DeniedNoMigrationClass;
		}

		if (!hasConformanceProof)
		{
			return SecureComputeVmReadVisibilityDecision::DeniedNoConformanceProof;
		}

		return SecureComputeVmReadVisibilityDecision::AllowedReadOnlyProjection;
	}
}
