#include "YAKSys_Hybrid_CPU.Core.DomainBindingTable.h"
#include "DomainBindingTable.h"
#include "../Services/YAKSys_Hybrid_CPU.Core.DomainRuntimeContext.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<DomainBindingResult> DomainBindingTable::Validate(std::shared_ptr<DomainBindingRequest> request)
	{
		if (request == nullptr || !request->Binding.has_value())
		{
			return DomainBindingResult::Denied(DomainBindingDecision::MissingBinding, L"Domain binding requires a binding entry.");
		}

		const DomainBindingEntry& binding = request->Binding.value();
		if (!binding.IsRuntimeAuthoritative())
		{
			return DomainBindingResult::Denied(DomainBindingDecision::RuntimeAuthorityRequired, L"Domain binding requires runtime authority.");
		}

		if (!binding.HasRuntimeContext())
		{
			return DomainBindingResult::Denied(DomainBindingDecision::MissingRuntimeContext, L"Domain binding requires a runtime context.");
		}

		if (!binding.HasRequiredDomains())
		{
			return DomainBindingResult::Denied(DomainBindingDecision::MissingRequiredDomains, L"Domain binding requires execution, memory, and I/O domain descriptors.");
		}

		if (request->ExpectedDomainId != 0 && binding.DomainId != request->ExpectedDomainId)
		{
			return DomainBindingResult::Denied(DomainBindingDecision::DomainIdMismatch, L"Domain binding did not match the expected domain id.");
		}

		if (request->RequiresCompatibilityProjection && !binding.CanProjectToCompatibilityFrontend())
		{
			return DomainBindingResult::Denied(DomainBindingDecision::CompatibilityProjectionDenied, L"Domain binding denies compatibility projection.");
		}

		return DomainBindingResult::Allowed();
	}

	bool DomainBindingTable::CanBind(std::shared_ptr<DomainBindingRequest> request)
	{
		return Validate(request)->IsAllowed;
	}

	bool DomainBindingEntry::HasRequiredDomains() const
	{
		return HasRuntimeContext() && Context->getHasRequiredDomains();
	}
}
