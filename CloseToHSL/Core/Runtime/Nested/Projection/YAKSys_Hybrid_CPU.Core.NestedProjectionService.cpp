#include "YAKSys_Hybrid_CPU.Core.NestedProjectionService.h"
#include "NestedProjectionService.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<NestedProjectionResult> NestedProjectionService::Validate(std::shared_ptr<NestedProjectionRequest> request)
	{
		if (request->Descriptor == nullptr)
		{
			return NestedProjectionResult::Denied(NestedProjectionDecision::MissingDescriptor, L"Nested projection requires a nested domain descriptor.");
		}

		if (!IsExplicitlyAllowed(request->RuntimeAdmission.IsAllowed, request->RuntimeAdmission.Reason))
		{
			return NestedProjectionResult::Denied(NestedProjectionDecision::RuntimeAdmissionDenied, !request->RuntimeAdmission.Reason.empty() ? request->RuntimeAdmission.Reason : L"Nested projection requires explicit runtime admission.");
		}

		if (!IsExplicitlyAllowed(request->CapabilityFilter.IsAllowed, request->CapabilityFilter.Reason))
		{
			return NestedProjectionResult::Denied(NestedProjectionDecision::CapabilityFilterDenied, !request->CapabilityFilter.Reason.empty() ? request->CapabilityFilter.Reason : L"Nested projection requires an explicit capability-filter allow.");
		}

		if (!IsExplicitlyAllowed(request->EvidencePolicy.IsAllowed, request->EvidencePolicy.Reason))
		{
			return NestedProjectionResult::Denied(NestedProjectionDecision::EvidencePolicyDenied, !request->EvidencePolicy.Reason.empty() ? request->EvidencePolicy.Reason : L"Nested projection requires an explicit evidence-policy allow.");
		}

		if (request->RequiresCompletionMapping && !IsExplicitlyAllowed(request->CompletionMapping.IsAllowed, request->CompletionMapping.Reason))
		{
			return NestedProjectionResult::Denied(NestedProjectionDecision::CompletionMappingDenied, !request->CompletionMapping.Reason.empty() ? request->CompletionMapping.Reason : L"Nested projection requires explicit completion mapping.");
		}

		if (request->RequiresCompatibilityProjection && !request->Descriptor->getCanProjectToCompatibilityFrontend())
		{
			return NestedProjectionResult::Denied(NestedProjectionDecision::CompatibilityProjectionDenied, L"Nested descriptor denies compatibility projection.");
		}

		return NestedProjectionResult::Allowed;
	}

	bool NestedProjectionService::CanProject(std::shared_ptr<NestedProjectionRequest> request)
	{
		return Validate(request)->IsAllowed;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static bool IsExplicitlyAllowed(bool allowed, string? reason)
	bool NestedProjectionService::IsExplicitlyAllowed(bool allowed, const std::wstring &reason)
	{
		return allowed && !StringHelper::isEmptyOrWhiteSpace(reason);
	}
}
