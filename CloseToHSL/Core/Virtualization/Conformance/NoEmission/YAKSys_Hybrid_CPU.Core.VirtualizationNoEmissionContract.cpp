#include "YAKSys_Hybrid_CPU.Core.VirtualizationNoEmissionContract.h"
#include "VirtualizationNoEmissionContract.h"

namespace YAKSys_Hybrid_CPU::Core
{

	VirtualizationNoEmissionContract::VirtualizationNoEmissionContract() : VirtualizationNoEmissionContract(std::make_shared<VirtualizationNoEmissionRegressionGate>())
	{
	}

	VirtualizationNoEmissionContract::VirtualizationNoEmissionContract(std::shared_ptr<VirtualizationNoEmissionRegressionGate> gate) : _gate(gate)
	{
	}

	std::shared_ptr<VirtualizationNoEmissionContractResult> VirtualizationNoEmissionContract::Validate(std::shared_ptr<VirtualizationNoEmissionContractRequest> request)
	{
		if (!request->GoldenArtifactCaptured)
		{
			return VirtualizationNoEmissionContractResult::Denied(VirtualizationNoEmissionContractDecision::MissingGoldenArtifact, L"No-emission conformance requires a golden artifact capture.");
		}

		if (request->Emission.EmitsHostOwnedEvidence)
		{
			return VirtualizationNoEmissionContractResult::Denied(VirtualizationNoEmissionContractDecision::HostEvidenceEmissionDenied, L"No-emission conformance rejects host-owned evidence emission.");
		}

		if (request->Emission.EmitsNativeLaneToken)
		{
			return VirtualizationNoEmissionContractResult::Denied(VirtualizationNoEmissionContractDecision::NativeTokenEmissionDenied, L"No-emission conformance rejects native lane token emission.");
		}

		if (!request->Emission.IsGeneratedProjection)
		{
			return VirtualizationNoEmissionContractResult::Denied(VirtualizationNoEmissionContractDecision::GeneratedProjectionRequired, L"No-emission conformance requires generated compatibility projection.");
		}

		if (!_gate->CanEmit(std::make_shared<VirtualizationEmissionRequest>(request->Emission)))
		{
			return VirtualizationNoEmissionContractResult::Denied(VirtualizationNoEmissionContractDecision::NoEmissionGateDenied, L"No-emission regression gate denied this emission request.");
		}

		return VirtualizationNoEmissionContractResult::Allowed;
	}

	bool VirtualizationNoEmissionContract::IsSatisfied(std::shared_ptr<VirtualizationNoEmissionContractRequest> request)
	{
		return Validate(request)->IsAllowed;
	}
}
