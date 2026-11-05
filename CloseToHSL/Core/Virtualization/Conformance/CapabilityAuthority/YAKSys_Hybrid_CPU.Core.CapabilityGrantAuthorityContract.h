#pragma once

#include "../../../Runtime/Capabilities/Grants/YAKSys_Hybrid_CPU.Core.CapabilityGrant.h"
#include "CapabilityGrantAuthorityContract.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class CapabilityGrantAuthorityContract final : public std::enable_shared_from_this<CapabilityGrantAuthorityContract>
	{
	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public CapabilityGrantAuthorityViolation Evaluate(CapabilityGrant? grant)
		CapabilityGrantAuthorityViolation Evaluate(std::shared_ptr<CapabilityGrant> grant);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public bool IsSatisfied(CapabilityGrant? grant)
		bool IsSatisfied(std::shared_ptr<CapabilityGrant> grant);
	};
}
