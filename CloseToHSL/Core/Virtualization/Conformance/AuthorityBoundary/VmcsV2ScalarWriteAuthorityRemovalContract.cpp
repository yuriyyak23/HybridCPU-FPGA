#include "VmcsV2ScalarWriteAuthorityRemovalContract.h"

namespace YAKSys_Hybrid_CPU::Core
{

	bool VmcsV2ScalarWriteAuthorityRemovalContract::getRemovedWithoutReplacement()
	{
		return true;
	}

	bool VmcsV2ScalarWriteAuthorityRemovalContract::getRejectsVmcsV2PublicScalarWriteAuthority()
	{
		return true;
	}

	bool VmcsV2ScalarWriteAuthorityRemovalContract::getKeepsVmcsV2DescriptorReadProjectionOnly()
	{
		return true;
	}

	bool VmcsV2ScalarWriteAuthorityRemovalContract::getRejectsVmcsFieldStoreReplacementOwner()
	{
		return true;
	}

	bool VmcsV2ScalarWriteAuthorityRemovalContract::getKeepsFrozenFieldSchemaVocabularyOnly()
	{
		return true;
	}

	std::span<const std::wstring> VmcsV2ScalarWriteAuthorityRemovalContract::getForbiddenDescriptorMutationMarkers()
	{
		return std::span<const std::wstring>(ForbiddenDescriptorMutationMarkerTable);
	}

	std::span<const std::wstring> VmcsV2ScalarWriteAuthorityRemovalContract::getForbiddenNewRuntimeOwnerMarkers()
	{
		return std::span<const std::wstring>(ForbiddenNewRuntimeOwnerMarkerTable);
	}
}
