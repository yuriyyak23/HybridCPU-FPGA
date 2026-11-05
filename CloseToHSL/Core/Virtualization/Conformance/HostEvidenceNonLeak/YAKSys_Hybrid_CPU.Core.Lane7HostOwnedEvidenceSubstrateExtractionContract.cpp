#include "YAKSys_Hybrid_CPU.Core.Lane7HostOwnedEvidenceSubstrateExtractionContract.h"

namespace YAKSys_Hybrid_CPU::Core
{

	bool Lane7HostOwnedEvidenceSubstrateExtractionContract::getExtractsLane7HostEvidenceToNeutralRuntimeNamespace()
	{
		return true;
	}

	bool Lane7HostOwnedEvidenceSubstrateExtractionContract::getRequiresTokenBackendAndSchedulerEvidenceRebuildAfterRestore()
	{
		return true;
	}

	bool Lane7HostOwnedEvidenceSubstrateExtractionContract::getKeepsVmxTranslationVmcsAndIotlbNamesFrozenCompatibilityOnly()
	{
		return true;
	}

	std::vector<std::wstring> Lane7HostOwnedEvidenceSubstrateExtractionContract::getForbiddenLane7StateCacheMarkers()
	{
		return ForbiddenLane7StateCacheMarkers;
	}

	std::vector<std::wstring> Lane7HostOwnedEvidenceSubstrateExtractionContract::getRequiredLane7StateDelegationMarkers()
	{
		return RequiredLane7StateDelegationMarkers;
	}

	std::vector<std::wstring> Lane7HostOwnedEvidenceSubstrateExtractionContract::getForbiddenNeutralEvidenceStoreMarkers()
	{
		return ForbiddenNeutralEvidenceStoreMarkers;
	}

	std::vector<std::wstring> Lane7HostOwnedEvidenceSubstrateExtractionContract::getRequiredNeutralEvidenceStoreMarkers()
	{
		return RequiredNeutralEvidenceStoreMarkers;
	}

	std::vector<std::wstring> Lane7HostOwnedEvidenceSubstrateExtractionContract::getForbiddenCheckpointEvidenceLeakMarkers()
	{
		return ForbiddenCheckpointEvidenceLeakMarkers;
	}

	std::vector<std::wstring> Lane7HostOwnedEvidenceSubstrateExtractionContract::getRequiredFrozenCompatibilityMarkers()
	{
		return RequiredFrozenCompatibilityMarkers;
	}

	bool Lane7HostOwnedEvidenceSubstrateExtractionContract::RejectsCheckpointNativeHandleProjection(std::shared_ptr<Lane7Checkpoint> checkpoint, std::shared_ptr<Execution::ExternalAccelerators::Tokens::AcceleratorTokenHandle> hostHandle)
	{
		return !checkpoint->ContainsNativeTokenHandle(hostHandle);
	}
}
