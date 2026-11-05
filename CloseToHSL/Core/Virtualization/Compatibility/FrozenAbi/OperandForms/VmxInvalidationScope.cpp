#include "VmxInvalidationScope.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<VmxInvalidationScopeAdmissionResult> VmxInvalidationScopeAdmissionPolicy::Validate(std::shared_ptr<VmxInvalidationScopeAdmissionRequest> request)
	{
		if (!IsSupportedScope(request->Scope))
		{
			return VmxInvalidationScopeAdmissionResult::Denied(VmxInvalidationScopeAdmissionDecision::UnsupportedScope, L"VMX invalidation scope is not a supported frozen ABI alias.");
		}

		if (!request->DescriptorValidated)
		{
			return VmxInvalidationScopeAdmissionResult::Denied(VmxInvalidationScopeAdmissionDecision::DescriptorValidationDenied, L"VMX invalidation requires descriptor validation.");
		}

		if (!request->CapabilityValidated)
		{
			return VmxInvalidationScopeAdmissionResult::Denied(VmxInvalidationScopeAdmissionDecision::CapabilityValidationDenied, L"VMX invalidation requires capability validation.");
		}

		if (!request->RuntimeInvalidationValidated)
		{
			return VmxInvalidationScopeAdmissionResult::Denied(VmxInvalidationScopeAdmissionDecision::RuntimeInvalidationDenied, L"VMX invalidation requires runtime-owned invalidation validation.");
		}

		if (!request->TranslationAuthorityValidated)
		{
			return VmxInvalidationScopeAdmissionResult::Denied(VmxInvalidationScopeAdmissionDecision::TranslationAuthorityDenied, L"VMX invalidation requires translation-domain authority validation.");
		}

		if (request->AttemptsDirectMmuMutation)
		{
			return VmxInvalidationScopeAdmissionResult::Denied(VmxInvalidationScopeAdmissionDecision::DirectMmuMutationDenied, L"VMX invalidation aliases cannot mutate MMU state directly.");
		}

		return VmxInvalidationScopeAdmissionResult::Allowed;
	}

	bool VmxInvalidationScopeAdmissionPolicy::CanAdmit(std::shared_ptr<VmxInvalidationScopeAdmissionRequest> request)
	{
		return Validate(request)->isAllowed();
	}

	bool VmxInvalidationScopeAdmissionPolicy::IsSupportedScope(VmxInvalidationScope scope)
	{
		return (scope == VmxInvalidationScope::SingleContext || scope == VmxInvalidationScope::AllContexts || scope == VmxInvalidationScope::SingleAddress);
	}
}
