#include "VmcsV2VectorStreamProjectionAuthorityRemovalContract.h"

namespace YAKSys_Hybrid_CPU::Core
{

	bool VmcsV2VectorStreamProjectionAuthorityRemovalContract::getRemovedWithoutReplacement()
	{
		return true;
	}

	bool VmcsV2VectorStreamProjectionAuthorityRemovalContract::getRejectsVmcsDescriptorOwnedVectorSaveRestore()
	{
		return true;
	}

	bool VmcsV2VectorStreamProjectionAuthorityRemovalContract::getRejectsVmcsDescriptorOwnedStreamValidationEvidence()
	{
		return true;
	}

	bool VmcsV2VectorStreamProjectionAuthorityRemovalContract::getKeepsFrozenVectorStreamVocabularyProjectionOnly()
	{
		return true;
	}

	bool VmcsV2VectorStreamProjectionAuthorityRemovalContract::getGenericVectorStreamRuntimeRemainsNeutralOwner()
	{
		return true;
	}

	std::span<const std::wstring> VmcsV2VectorStreamProjectionAuthorityRemovalContract::getRemovedPaths()
	{
		return std::span<const std::wstring>(RemovedPathTable);
	}

	std::span<const std::wstring> VmcsV2VectorStreamProjectionAuthorityRemovalContract::getForbiddenProductionOwnerMarkers()
	{
		return std::span<const std::wstring>(ForbiddenProductionOwnerMarkerTable);
	}

	std::span<const std::wstring> VmcsV2VectorStreamProjectionAuthorityRemovalContract::getForbiddenNewRuntimeOwnerMarkers()
	{
		return std::span<const std::wstring>(ForbiddenNewRuntimeOwnerMarkerTable);
	}
}
