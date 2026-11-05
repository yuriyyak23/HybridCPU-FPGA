#include "YAKSys_Hybrid_CPU.Core.GenericDomainSubstrateExtractionContract.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::vector<std::wstring> GenericDomainSubstrateExtractionContract::getRemovedGenericDomainSubstratePaths()
	{
		return RemovedGenericDomainSubstratePaths;
	}

	std::vector<std::wstring> GenericDomainSubstrateExtractionContract::getNeutralRuntimePaths()
	{
		return NeutralRuntimePaths;
	}

	std::vector<std::wstring> GenericDomainSubstrateExtractionContract::getForbiddenNeutralRuntimeMarkers()
	{
		return ForbiddenNeutralRuntimeMarkers;
	}

	std::vector<std::wstring> GenericDomainSubstrateExtractionContract::getRequiredNeutralRuntimeMarkers()
	{
		return RequiredNeutralRuntimeMarkers;
	}

	std::vector<std::wstring> GenericDomainSubstrateExtractionContract::getFrozenCompatibilityProjectionPaths()
	{
		return FrozenCompatibilityProjectionPaths;
	}

	std::vector<std::wstring> GenericDomainSubstrateExtractionContract::getRequiredFrozenCompatibilityMarkers()
	{
		return RequiredFrozenCompatibilityMarkers;
	}
}
