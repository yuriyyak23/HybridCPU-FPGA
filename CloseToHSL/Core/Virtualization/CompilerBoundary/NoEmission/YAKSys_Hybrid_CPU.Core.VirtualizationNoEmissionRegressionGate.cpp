#include "YAKSys_Hybrid_CPU.Core.VirtualizationNoEmissionRegressionGate.h"

namespace YAKSys_Hybrid_CPU::Core
{

	VirtualizationEmissionDecision VirtualizationNoEmissionRegressionGate::Evaluate(std::shared_ptr<VirtualizationEmissionRequest> request)
	{
		if (request->EmitsHostOwnedEvidence)
		{
			return VirtualizationEmissionDecision::DeniedHostEvidenceEmission;
		}

		if (request->EmitsNativeLaneToken)
		{
			return VirtualizationEmissionDecision::DeniedNativeTokenEmission;
		}

		if (!request->DescriptorValidated)
		{
			return VirtualizationEmissionDecision::DeniedUnvalidatedDescriptorEmission;
		}

		if (!request->IsCompatibilityFrontend || !request->IsGeneratedProjection)
		{
			return VirtualizationEmissionDecision::DeniedDirectSubstrateEmission;
		}

		return VirtualizationEmissionDecision::AllowedCompatibilityProjection;
	}

	bool VirtualizationNoEmissionRegressionGate::CanEmit(std::shared_ptr<VirtualizationEmissionRequest> request)
	{
		return Evaluate(request) == VirtualizationEmissionDecision::AllowedCompatibilityProjection;
	}
}
