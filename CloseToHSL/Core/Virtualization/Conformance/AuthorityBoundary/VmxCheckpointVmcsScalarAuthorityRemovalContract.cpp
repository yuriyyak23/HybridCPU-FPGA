#include "VmxCheckpointVmcsScalarAuthorityRemovalContract.h"

namespace YAKSys_Hybrid_CPU::Core
{

	bool VmxCheckpointVmcsScalarAuthorityRemovalContract::getRemovedWithoutReplacement()
	{
		return true;
	}

	bool VmxCheckpointVmcsScalarAuthorityRemovalContract::getRejectsVmcsScalarCheckpointImageAuthority()
	{
		return true;
	}

	bool VmxCheckpointVmcsScalarAuthorityRemovalContract::getRejectsVmcsDescriptorScalarRestoreAuthority()
	{
		return true;
	}

	bool VmxCheckpointVmcsScalarAuthorityRemovalContract::getKeepsVmcsVocabularyProjectionOnly()
	{
		return true;
	}

	bool VmxCheckpointVmcsScalarAuthorityRemovalContract::getGenericDomainCheckpointRemainsNeutralOwner()
	{
		return true;
	}

	std::span<const std::wstring> VmxCheckpointVmcsScalarAuthorityRemovalContract::getRemovedPaths()
	{
		return std::span<const std::wstring>(RemovedPathTable);
	}

	std::span<const std::wstring> VmxCheckpointVmcsScalarAuthorityRemovalContract::getForbiddenProductionOwnerMarkers()
	{
		return std::span<const std::wstring>(ForbiddenProductionOwnerMarkerTable);
	}

	std::span<const std::wstring> VmxCheckpointVmcsScalarAuthorityRemovalContract::getForbiddenNewRuntimeOwnerMarkers()
	{
		return std::span<const std::wstring>(ForbiddenNewRuntimeOwnerMarkerTable);
	}
}
