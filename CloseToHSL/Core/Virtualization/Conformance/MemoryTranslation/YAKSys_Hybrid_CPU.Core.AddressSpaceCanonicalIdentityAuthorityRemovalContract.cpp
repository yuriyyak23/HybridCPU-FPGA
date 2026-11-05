#include "YAKSys_Hybrid_CPU.Core.AddressSpaceCanonicalIdentityAuthorityRemovalContract.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::vector<std::wstring> AddressSpaceCanonicalIdentityAuthorityRemovalContract::getForbiddenCanonicalIdentityMarkers()
	{
		return ForbiddenCanonicalIdentityMarkers;
	}

	std::vector<std::wstring> AddressSpaceCanonicalIdentityAuthorityRemovalContract::getForbiddenNestedCompositionMarkers()
	{
		return ForbiddenNestedCompositionMarkers;
	}

	bool AddressSpaceCanonicalIdentityAuthorityRemovalContract::IsNeutralRuntimeIdentity(std::shared_ptr<MemoryDomainTranslationControl> control, unsigned long long secondStageEpoch, unsigned long long addressSpaceTagEpoch, std::shared_ptr<YAKSys_Hybrid_CPU::Memory::AddressSpaceId> identity)
	{
		if (control == nullptr || identity == nullptr)
		{
			return false;
		}
		const auto expected = control->ToAddressSpaceId(secondStageEpoch, addressSpaceTagEpoch);
		return identity->DomainTag == expected.DomainTag && identity->AddressSpaceTag == expected.AddressSpaceTag && identity->SecondStageRootIdentity == expected.SecondStageRootIdentity && identity->SecondStageEpoch == expected.SecondStageEpoch && identity->AddressSpaceTagEpoch == expected.AddressSpaceTagEpoch && identity->AddressSpaceGeneration == expected.AddressSpaceGeneration;
	}
}
