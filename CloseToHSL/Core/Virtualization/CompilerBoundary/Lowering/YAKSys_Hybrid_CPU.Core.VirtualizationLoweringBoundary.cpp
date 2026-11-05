#include "YAKSys_Hybrid_CPU.Core.VirtualizationLoweringBoundary.h"
#include "VirtualizationLoweringBoundary.h"
#include "../NoEmission/VirtualizationNoEmissionRegressionGate.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<VirtualizationLoweringResult> VirtualizationLoweringBoundary::ValidateLowering(std::shared_ptr<VirtualizationLoweringRequest> request)
	{
		if (!request->IntentResult->IsAllowed)
		{
			return VirtualizationLoweringResult::Denied(VirtualizationLoweringDecision::IntentDenied, L"Virtualization lowering requires an allowed compiler intent.");
		}

		if (!request->DescriptorValidated)
		{
			return VirtualizationLoweringResult::Denied(VirtualizationLoweringDecision::DescriptorValidationDenied, L"Virtualization lowering requires descriptor validation.");
		}

		if (!request->CapabilityValidated)
		{
			return VirtualizationLoweringResult::Denied(VirtualizationLoweringDecision::CapabilityValidationDenied, L"Virtualization lowering requires capability validation.");
		}

		if (!request->RuntimeValidated)
		{
			return VirtualizationLoweringResult::Denied(VirtualizationLoweringDecision::RuntimeValidationDenied, L"Virtualization lowering requires runtime validation.");
		}

		if (request->EmissionDecision != VirtualizationEmissionDecision::AllowedCompatibilityProjection)
		{
			return VirtualizationLoweringResult::Denied(VirtualizationLoweringDecision::NoEmissionDenied, L"Virtualization lowering requires the no-emission regression gate.");
		}

		if (request->EmitsDirectVmxHandler)
		{
			return VirtualizationLoweringResult::Denied(VirtualizationLoweringDecision::DirectHandlerEmissionDenied, L"Virtualization lowering cannot emit direct VMX handler calls.");
		}

		return VirtualizationLoweringResult::Allowed();
	}
}
