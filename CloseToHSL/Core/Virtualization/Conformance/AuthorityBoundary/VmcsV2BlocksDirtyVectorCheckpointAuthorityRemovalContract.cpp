#include "VmcsV2BlocksDirtyVectorCheckpointAuthorityRemovalContract.h"

namespace YAKSys_Hybrid_CPU::Core
{

	bool VmcsV2BlocksDirtyVectorCheckpointAuthorityRemovalContract::getRemovedWithoutReplacement()
	{
		return true;
	}

	bool VmcsV2BlocksDirtyVectorCheckpointAuthorityRemovalContract::getRejectsVmcsBlocksDirtyLogMutationAuthority()
	{
		return true;
	}

	bool VmcsV2BlocksDirtyVectorCheckpointAuthorityRemovalContract::getRejectsVmcsBlocksVectorSaveRestoreAuthority()
	{
		return true;
	}

	bool VmcsV2BlocksDirtyVectorCheckpointAuthorityRemovalContract::getRejectsVmcsBlocksCheckpointRestoreAuthority()
	{
		return true;
	}

	bool VmcsV2BlocksDirtyVectorCheckpointAuthorityRemovalContract::getKeepsVmcsBlocksVocabularyProjectionOnly()
	{
		return true;
	}

	bool VmcsV2BlocksDirtyVectorCheckpointAuthorityRemovalContract::getGenericVectorStreamRuntimeRemainsNeutralOwner()
	{
		return true;
	}

	bool VmcsV2BlocksDirtyVectorCheckpointAuthorityRemovalContract::getGenericDomainCheckpointRemainsNeutralOwner()
	{
		return true;
	}

	bool VmcsV2BlocksDirtyVectorCheckpointAuthorityRemovalContract::getCompatibilityDirtyAccountingRemainsFailClosed()
	{
		return true;
	}

	const std::vector<std::wstring> &VmcsV2BlocksDirtyVectorCheckpointAuthorityRemovalContract::getForbiddenVmcsBlocksHelperMarkers()
	{
		return ForbiddenVmcsBlocksHelperMarkerTable;
	}

	const std::vector<std::wstring> &VmcsV2BlocksDirtyVectorCheckpointAuthorityRemovalContract::getForbiddenDescriptorHelperMarkers()
	{
		return ForbiddenDescriptorHelperMarkerTable;
	}

	const std::vector<std::wstring> &VmcsV2BlocksDirtyVectorCheckpointAuthorityRemovalContract::getForbiddenNewRuntimeOwnerMarkers()
	{
		return ForbiddenNewRuntimeOwnerMarkerTable;
	}
}
