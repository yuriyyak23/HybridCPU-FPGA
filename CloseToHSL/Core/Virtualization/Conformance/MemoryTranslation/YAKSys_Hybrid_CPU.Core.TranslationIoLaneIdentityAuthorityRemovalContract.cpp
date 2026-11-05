#include "YAKSys_Hybrid_CPU.Core.TranslationIoLaneIdentityAuthorityRemovalContract.h"

using namespace HybridCPU_ISE::CloseToHSL::Memory::MMU;
using namespace YAKSys_Hybrid_CPU::Memory;

namespace YAKSys_Hybrid_CPU::Core
{

	std::vector<std::wstring> TranslationIoLaneIdentityAuthorityRemovalContract::getIoIdentityPaths()
	{
		return IoIdentityPaths;
	}

	std::vector<std::wstring> TranslationIoLaneIdentityAuthorityRemovalContract::getLaneIdentityPaths()
	{
		return LaneIdentityPaths;
	}

	std::vector<std::wstring> TranslationIoLaneIdentityAuthorityRemovalContract::getForbiddenExecutableIdentityMarkers()
	{
		return ForbiddenExecutableIdentityMarkers;
	}

	std::vector<std::wstring> TranslationIoLaneIdentityAuthorityRemovalContract::getForbiddenNestedResultMarkers()
	{
		return ForbiddenNestedResultMarkers;
	}

	std::vector<std::wstring> TranslationIoLaneIdentityAuthorityRemovalContract::getForbiddenCompatibilityFactoryMarkers()
	{
		return ForbiddenCompatibilityFactoryMarkers;
	}

	bool TranslationIoLaneIdentityAuthorityRemovalContract::IsNeutralIotlbIdentity(std::shared_ptr<IommuDomainBinding> binding, std::shared_ptr<IotlbTag> tag)
	{
		return binding != nullptr && tag != nullptr && binding->isValid() && tag->IoDomainTag == binding->IoDomainTag && tag->DomainId == binding->DomainId && tag->DeviceId == binding->DeviceId && tag->DomainEpoch == binding->DomainEpoch;
	}
}
