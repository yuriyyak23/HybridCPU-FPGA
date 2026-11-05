#include "YAKSys_Hybrid_CPU.Core.CapabilityRuntimeSubstrateExtractionContract.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::vector<std::wstring> CapabilityRuntimeSubstrateExtractionContract::getRemovedCapabilitySubstratePaths()
	{
		return RemovedCapabilitySubstratePaths;
	}

	std::vector<std::wstring> CapabilityRuntimeSubstrateExtractionContract::getNeutralCapabilityRuntimePaths()
	{
		return NeutralCapabilityRuntimePaths;
	}

	std::vector<std::wstring> CapabilityRuntimeSubstrateExtractionContract::getForbiddenNeutralCapabilityRuntimeMarkers()
	{
		return ForbiddenNeutralCapabilityRuntimeMarkers;
	}

	std::vector<std::wstring> CapabilityRuntimeSubstrateExtractionContract::getRequiredNeutralCapabilityRuntimeMarkers()
	{
		return RequiredNeutralCapabilityRuntimeMarkers;
	}

	std::vector<std::wstring> CapabilityRuntimeSubstrateExtractionContract::getRequiredCompatibilityProjectionMarkers()
	{
		return RequiredCompatibilityProjectionMarkers;
	}

	std::vector<std::wstring> CapabilityRuntimeSubstrateExtractionContract::getRemovedProjectFolderMarkers()
	{
		return RemovedProjectFolderMarkers;
	}

	bool CapabilityRuntimeSubstrateExtractionContract::getMovesTypedCapabilityRuntimeOutOfVmxSubstrate() const
	{
		return true;
	}

	bool CapabilityRuntimeSubstrateExtractionContract::getKeepsMaskIngressCompatibilityProjectionOnly() const
	{
		return true;
	}

	bool CapabilityRuntimeSubstrateExtractionContract::getDoesNotCreateVmxCapabilityOwner() const
	{
		return true;
	}

	bool CapabilityRuntimeSubstrateExtractionContract::IsSatisfied()
	{
		return getMovesTypedCapabilityRuntimeOutOfVmxSubstrate() && getKeepsMaskIngressCompatibilityProjectionOnly() && getDoesNotCreateVmxCapabilityOwner();
	}
}
