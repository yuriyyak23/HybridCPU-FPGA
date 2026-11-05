#include "YAKSys_Hybrid_CPU.Core.Lane6HostOwnedEvidenceBoundaryRemovalContract.h"

namespace YAKSys_Hybrid_CPU::Core
{

	bool Lane6HostOwnedEvidenceBoundaryRemovalContract::getRemovedVmxShapedHostTokenCarrier()
	{
		return true;
	}

	bool Lane6HostOwnedEvidenceBoundaryRemovalContract::getRequiresHostEvidenceRebuildAfterRestore()
	{
		return true;
	}

	bool Lane6HostOwnedEvidenceBoundaryRemovalContract::getRejectsEpochWraparound()
	{
		return true;
	}

	std::vector<std::wstring> Lane6HostOwnedEvidenceBoundaryRemovalContract::getForbiddenNeutralEvidenceStoreMarkers()
	{
		return ForbiddenNeutralEvidenceStoreMarkers;
	}

	std::vector<std::wstring> Lane6HostOwnedEvidenceBoundaryRemovalContract::getForbiddenLane6StateOwnerMarkers()
	{
		return ForbiddenLane6StateOwnerMarkers;
	}

	std::vector<std::wstring> Lane6HostOwnedEvidenceBoundaryRemovalContract::getRequiredRebuildMarkers()
	{
		return RequiredRebuildMarkers;
	}

	std::vector<std::wstring> Lane6HostOwnedEvidenceBoundaryRemovalContract::getRequiredFailClosedEpochMarkers()
	{
		return RequiredFailClosedEpochMarkers;
	}
}
