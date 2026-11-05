#include "VmcsV2ScalarProjectionCacheAuthorityRemovalContract.h"

namespace YAKSys_Hybrid_CPU::Core
{

	bool VmcsV2ScalarProjectionCacheAuthorityRemovalContract::getRemovedWithoutReplacement()
	{
		return true;
	}

	bool VmcsV2ScalarProjectionCacheAuthorityRemovalContract::getRejectsDescriptorOwnedScalarProjectionCache()
	{
		return true;
	}

	bool VmcsV2ScalarProjectionCacheAuthorityRemovalContract::getKeepsTryReadScalarFieldAsDeniedCompatibilityAbi()
	{
		return true;
	}

	bool VmcsV2ScalarProjectionCacheAuthorityRemovalContract::getRequiresGeneratedReadOnlyProjectionOverNeutralOwners()
	{
		return true;
	}

	bool VmcsV2ScalarProjectionCacheAuthorityRemovalContract::getRejectsScalarProjectionStoreReplacementOwner()
	{
		return true;
	}

	std::span<const std::wstring> VmcsV2ScalarProjectionCacheAuthorityRemovalContract::getForbiddenDescriptorCacheMarkers()
	{
		return std::span<const std::wstring>(ForbiddenDescriptorCacheMarkerTable);
	}

	std::span<const std::wstring> VmcsV2ScalarProjectionCacheAuthorityRemovalContract::getForbiddenNewRuntimeOwnerMarkers()
	{
		return std::span<const std::wstring>(ForbiddenNewRuntimeOwnerMarkerTable);
	}
}
