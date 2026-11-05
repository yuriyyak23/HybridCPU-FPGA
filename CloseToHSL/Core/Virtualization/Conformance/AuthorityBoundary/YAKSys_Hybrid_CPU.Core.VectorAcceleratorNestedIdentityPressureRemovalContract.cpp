#include "YAKSys_Hybrid_CPU.Core.VectorAcceleratorNestedIdentityPressureRemovalContract.h"

#include "../../../Runtime/Migration/Checkpoint/YAKSys_Hybrid_CPU.Core.DomainCheckpointImage.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::vector<std::wstring> VectorAcceleratorNestedIdentityPressureRemovalContract::getRemovedVectorIdentityPaths()
	{
		return RemovedVectorIdentityPaths;
	}

	std::vector<std::wstring> VectorAcceleratorNestedIdentityPressureRemovalContract::getRemovedAcceleratorWrapperPaths()
	{
		return RemovedAcceleratorWrapperPaths;
	}

	std::vector<std::wstring> VectorAcceleratorNestedIdentityPressureRemovalContract::getNeutralVectorIdentityPaths()
	{
		return NeutralVectorIdentityPaths;
	}

	std::vector<std::wstring> VectorAcceleratorNestedIdentityPressureRemovalContract::getNeutralAcceleratorNamespacePaths()
	{
		return NeutralAcceleratorNamespacePaths;
	}

	std::vector<std::wstring> VectorAcceleratorNestedIdentityPressureRemovalContract::getForbiddenVectorIdentityMarkers()
	{
		return ForbiddenVectorIdentityMarkers;
	}

	std::vector<std::wstring> VectorAcceleratorNestedIdentityPressureRemovalContract::getForbiddenAcceleratorNamespaceMarkers()
	{
		return ForbiddenAcceleratorNamespaceMarkers;
	}

	std::vector<std::wstring> VectorAcceleratorNestedIdentityPressureRemovalContract::getForbiddenNestedIntentMarkers()
	{
		return ForbiddenNestedIntentMarkers;
	}

	std::vector<std::wstring> VectorAcceleratorNestedIdentityPressureRemovalContract::getRequiredNeutralNestedIntentMarkers()
	{
		return RequiredNeutralNestedIntentMarkers;
	}

	bool VectorAcceleratorNestedIdentityPressureRemovalContract::IsNeutralVectorExceptionIdentity(std::shared_ptr<VectorStreamExceptionInfo> info, unsigned short executionDomainTag, unsigned short addressSpaceTag)
	{
		return info && info->ExecutionDomainTag == executionDomainTag && info->AddressSpaceTag == addressSpaceTag && info->RequiresCompatibilityExitProjection;
	}

	bool VectorAcceleratorNestedIdentityPressureRemovalContract::IsNeutralStreamFaultIdentity(std::shared_ptr<VectorStreamDescriptorFaultInfo> info, unsigned short executionDomainTag, unsigned short addressSpaceTag)
	{
		return info && info->ExecutionDomainTag == executionDomainTag && info->AddressSpaceTag == addressSpaceTag && info->IsFaulted;
	}

	bool VectorAcceleratorNestedIdentityPressureRemovalContract::RejectsHostOwnedNestedRestore(std::shared_ptr<NestedDomainProjectionCheckpointService> service, std::shared_ptr<NestedDomainProjectionCheckpointRequest> request)
	{
		if (!service || !request || !request->Checkpoint)
		{
			return false;
		}

		const auto result = service->Validate(request);
		return result && !result->IsAllowed && request->Checkpoint->getContainsHostOwnedEvidence();
	}
}
