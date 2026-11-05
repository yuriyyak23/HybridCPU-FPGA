#include "YAKSys_Hybrid_CPU.Core.SecureComputeCompatibilityBoundary.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<SecureComputeCompatibilityBoundaryResult> SecureComputeCompatibilityBoundary::Admit(SecureComputeCompatibilityOperation operation, bool projectionOnly)
	{
		switch (operation)
		{
		case SecureComputeCompatibilityOperation::ReadProjection:
			if (projectionOnly) return SecureComputeCompatibilityBoundaryResult::AllowedProjectionOnly;
			break;
		case SecureComputeCompatibilityOperation::ActivateSecureCompute:
			return std::make_shared<SecureComputeCompatibilityBoundaryResult>(SecureComputeCompatibilityBoundaryDecision::DeniedVmxActivation, L"VMX cannot activate SecureCompute.");
		case SecureComputeCompatibilityOperation::GrantSecureCompute:
			return std::make_shared<SecureComputeCompatibilityBoundaryResult>(SecureComputeCompatibilityBoundaryDecision::DeniedVmxCapsAuthority, L"VmxCaps cannot grant SecureCompute authority.");
		case SecureComputeCompatibilityOperation::StoreSecureState:
			return std::make_shared<SecureComputeCompatibilityBoundaryResult>(SecureComputeCompatibilityBoundaryDecision::DeniedVmcsStateStore, L"VMCS cannot store SecureCompute state.");
		case SecureComputeCompatibilityOperation::WriteProjection:
			return std::make_shared<SecureComputeCompatibilityBoundaryResult>(SecureComputeCompatibilityBoundaryDecision::DeniedWriteMutation, L"Compatibility writes cannot mutate SecureCompute state.");
		default:
			break;
		}
		return std::make_shared<SecureComputeCompatibilityBoundaryResult>(SecureComputeCompatibilityBoundaryDecision::DeniedByDefault, L"SecureCompute compatibility operation is denied by default.");
	}
}
