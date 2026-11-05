#pragma once

#include "SecureComputeVmReadVisibilityPolicy.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class SecureComputeVmReadVisibilityPolicy final : public std::enable_shared_from_this<SecureComputeVmReadVisibilityPolicy>
	{
	public:
		SecureComputeVmReadVisibilityDecision Validate(bool hasNeutralOwner, bool hasReadOnlySource, bool hasSecureVisibility, bool hasMigrationClassification, bool hasConformanceProof);
	};
}
