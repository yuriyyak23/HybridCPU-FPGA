#include "HypercallRuntimeOwnerRegistry.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<HypercallRuntimeOwnerAllocation> HypercallRuntimeOwnerRegistry::getPhase38ProbeOwner()
	{
		static const auto owner = std::make_shared<HypercallRuntimeOwnerAllocation>(VirtualizationDecisionValidatorV2::ExpectedOwnerId, 1, 1, VirtualizationDecisionValidatorV2::ExpectedDecisionId, Phase38VirtualizationDecisionAcceptanceV2::ExpectedSpecDigest, L"DomainHypercallRuntimeOwner");
		return owner;
	}

	bool HypercallRuntimeOwnerRegistry::TryGetAllocation(unsigned long long ownerId, std::shared_ptr<HypercallRuntimeOwnerAllocation> &allocation)
	{
		if (ownerId == getPhase38ProbeOwner()->OwnerId)
		{
			allocation = getPhase38ProbeOwner();
			return true;
		}

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: allocation = null!;
		allocation = nullptr;
		return false;
	}
}
