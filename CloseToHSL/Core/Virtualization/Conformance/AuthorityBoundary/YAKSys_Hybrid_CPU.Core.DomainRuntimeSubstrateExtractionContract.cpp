#include "YAKSys_Hybrid_CPU.Core.DomainRuntimeSubstrateExtractionContract.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::vector<std::wstring> DomainRuntimeSubstrateExtractionContract::getRemovedDomainRuntimeSubstratePaths()
	{
		return RemovedDomainRuntimeSubstratePaths;
	}

	std::vector<std::wstring> DomainRuntimeSubstrateExtractionContract::getNeutralDomainRuntimePaths()
	{
		return NeutralDomainRuntimePaths;
	}

	std::vector<std::wstring> DomainRuntimeSubstrateExtractionContract::getFrozenCompatibilityVocabularyPaths()
	{
		return FrozenCompatibilityVocabularyPaths;
	}

	std::vector<std::wstring> DomainRuntimeSubstrateExtractionContract::getForbiddenNeutralDomainRuntimeMarkers()
	{
		return ForbiddenNeutralDomainRuntimeMarkers;
	}

	std::vector<std::wstring> DomainRuntimeSubstrateExtractionContract::getRequiredNeutralDomainRuntimeMarkers()
	{
		return RequiredNeutralDomainRuntimeMarkers;
	}

	std::vector<std::wstring> DomainRuntimeSubstrateExtractionContract::getRequiredRuntimeAdmissionMarkers()
	{
		return RequiredRuntimeAdmissionMarkers;
	}

	std::vector<std::wstring> DomainRuntimeSubstrateExtractionContract::getRequiredFrozenCompatibilityMarkers()
	{
		return RequiredFrozenCompatibilityMarkers;
	}
}
