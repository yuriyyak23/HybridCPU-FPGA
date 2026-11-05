#include "VmxFunctionLeaf.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<VmxFunctionLeafAdmissionResult> VmxFunctionLeafAdmissionPolicy::Validate(std::shared_ptr<VmxFunctionLeafAdmissionRequest> request)
	{
		if (!IsSupportedLeaf(request->Leaf))
		{
			return VmxFunctionLeafAdmissionResult::Denied(VmxFunctionLeafAdmissionDecision::UnsupportedLeaf, L"VMFUNC leaf is not declared in the frozen compatibility alias set.");
		}

		if (!request->IsFrozenAbiSurface)
		{
			return VmxFunctionLeafAdmissionResult::Denied(VmxFunctionLeafAdmissionDecision::FrozenAbiRequired, L"VMFUNC leaves are valid only on the frozen compatibility ABI surface.");
		}

		if (!request->DescriptorValidated)
		{
			return VmxFunctionLeafAdmissionResult::Denied(VmxFunctionLeafAdmissionDecision::DescriptorValidationDenied, L"VMFUNC leaf admission requires descriptor validation.");
		}

		if (!request->CapabilityValidated)
		{
			return VmxFunctionLeafAdmissionResult::Denied(VmxFunctionLeafAdmissionDecision::CapabilityValidationDenied, L"VMFUNC leaf admission requires capability validation.");
		}

		if (RequiresLane7Grant(request->Leaf) && !request->Lane7GrantValidated)
		{
			return VmxFunctionLeafAdmissionResult::Denied(VmxFunctionLeafAdmissionDecision::Lane7GrantDenied, L"Lane7 VMFUNC leaves require an explicit Lane7 grant.");
		}

		if (request->UsesFastPath && !request->CapabilityValidated)
		{
			return VmxFunctionLeafAdmissionResult::Denied(VmxFunctionLeafAdmissionDecision::FastPathGrantDenied, L"VMFUNC fast paths require prevalidated capability grants.");
		}

		return VmxFunctionLeafAdmissionResult::Allowed;
	}

	bool VmxFunctionLeafAdmissionPolicy::CanAdmit(std::shared_ptr<VmxFunctionLeafAdmissionRequest> request)
	{
		return Validate(request)->isAllowed();
	}

	bool VmxFunctionLeafAdmissionPolicy::IsSupportedLeaf(VmxFunctionLeaf leaf)
	{
		return (leaf == VmxFunctionLeaf::CapabilityQuery || leaf == VmxFunctionLeaf::Lane7QueryCaps || leaf == VmxFunctionLeaf::Lane7Submit);
	}

	bool VmxFunctionLeafAdmissionPolicy::RequiresLane7Grant(VmxFunctionLeaf leaf)
	{
		return (leaf == VmxFunctionLeaf::Lane7QueryCaps || leaf == VmxFunctionLeaf::Lane7Submit);
	}
}
