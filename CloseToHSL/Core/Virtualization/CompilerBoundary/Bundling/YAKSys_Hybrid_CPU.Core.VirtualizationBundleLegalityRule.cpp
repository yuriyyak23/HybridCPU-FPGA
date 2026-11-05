#include "YAKSys_Hybrid_CPU.Core.VirtualizationBundleLegalityRule.h"
#include "VirtualizationBundleLegalityRule.h"
#include "../LaneBinding/VirtualizationLaneBindingPolicy.h"
#include "../Scheduling/VirtualizationSystemSingletonSchedulingRule.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<VirtualizationBundleLegalityResult> VirtualizationBundleLegalityRule::Evaluate(std::shared_ptr<VirtualizationBundleLegalityRequest> request)
	{
		if (request->Descriptor == nullptr)
		{
			return VirtualizationBundleLegalityResult::Denied(VirtualizationBundleLegalityDecision::MissingDescriptor, L"Bundle legality requires an execution-domain descriptor.");
		}

		if (!request->DescriptorValidated)
		{
			return VirtualizationBundleLegalityResult::Denied(VirtualizationBundleLegalityDecision::DescriptorValidationDenied, L"Bundle legality cannot be evaluated from an unvalidated descriptor.");
		}

		if (!request->Descriptor->isRuntimeAuthoritative())
		{
			return VirtualizationBundleLegalityResult::Denied(VirtualizationBundleLegalityDecision::RuntimeAuthorityRequired, L"Runtime-owned bundle legality is required.");
		}

		if (request->Descriptor->getRequiresRuntimeValidation() && !request->RuntimeValidated)
		{
			return VirtualizationBundleLegalityResult::Denied(VirtualizationBundleLegalityDecision::RuntimeValidationDenied, L"Runtime validation is required before bundling.");
		}

		if (request->UsesCompilerEvidenceAsAuthority || request->Descriptor->getCanUseCompilerEvidenceAsAuthority())
		{
			return VirtualizationBundleLegalityResult::Denied(VirtualizationBundleLegalityDecision::CompilerEvidenceAsAuthorityDenied, L"Compiler evidence may accompany bundling but cannot be bundle-legality authority.");
		}

		if (request->RequiresCompatibilityProjection && !request->Descriptor->getCanProjectToCompatibilityFrontend())
		{
			return VirtualizationBundleLegalityResult::Denied(VirtualizationBundleLegalityDecision::CompatibilityProjectionDenied, L"Compatibility projection is only allowed from runtime-authoritative descriptors.");
		}

		if (request->LaneBindingDecision != VirtualizationLaneBindingDecision::Allowed)
		{
			return VirtualizationBundleLegalityResult::Denied(VirtualizationBundleLegalityDecision::LaneBindingDenied, L"Lane binding denied the virtualization bundle placement.");
		}

		if (request->SchedulingDecision != VirtualizationSchedulingDecision::Allowed)
		{
			return VirtualizationBundleLegalityResult::Denied(VirtualizationBundleLegalityDecision::SchedulingDenied, L"System-singleton scheduling denied the virtualization bundle placement.");
		}

		return VirtualizationBundleLegalityResult::Allowed();
	}

	bool VirtualizationBundleLegalityRule::CanBundle(std::shared_ptr<VirtualizationBundleLegalityRequest> request)
	{
		return Evaluate(request)->IsAllowed;
	}
}
