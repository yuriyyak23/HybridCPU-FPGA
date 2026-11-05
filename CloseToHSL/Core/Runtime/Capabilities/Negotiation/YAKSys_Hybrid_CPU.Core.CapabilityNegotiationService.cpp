#include "YAKSys_Hybrid_CPU.Core.CapabilityNegotiationService.h"
#include "CapabilityNegotiationService.h"

namespace YAKSys_Hybrid_CPU::Core
{

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public CapabilityNegotiationResult Negotiate(CapabilityDescriptorSet? descriptorSet, ulong capabilityMask, CapabilityGrantScope scope = CapabilityGrantScope.DomainGranted)
	std::shared_ptr<CapabilityNegotiationResult> CapabilityNegotiationService::Negotiate(std::shared_ptr<CapabilityDescriptorSet> descriptorSet, unsigned long long capabilityMask, CapabilityGrantScope scope)
	{
		return Negotiate(descriptorSet, capabilityMask, CapabilityGrant::DefaultRuntimeOwnerDomainId, scope);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public CapabilityNegotiationResult Negotiate(CapabilityDescriptorSet? descriptorSet, ulong capabilityMask, ulong ownerDomainId, CapabilityGrantScope scope = CapabilityGrantScope.DomainGranted)
	std::shared_ptr<CapabilityNegotiationResult> CapabilityNegotiationService::Negotiate(std::shared_ptr<CapabilityDescriptorSet> descriptorSet, unsigned long long capabilityMask, unsigned long long ownerDomainId, CapabilityGrantScope scope)
	{
		if (capabilityMask == 0)
		{
			return CapabilityNegotiationResult::Denied(CapabilityNegotiationDecision::DeniedEmptyMask);
		}

		if (descriptorSet == nullptr)
		{
			return CapabilityNegotiationResult::Denied(CapabilityNegotiationDecision::DeniedMissingDescriptorSet);
		}

		if (!descriptorSet->HasHardwareCapability(capabilityMask))
		{
			return CapabilityNegotiationResult::Denied(CapabilityNegotiationDecision::DeniedMissingHardwareCapability);
		}

		if (!descriptorSet->IsRuntimeEnabled(capabilityMask))
		{
			return CapabilityNegotiationResult::Denied(CapabilityNegotiationDecision::DeniedRuntimeDisabled);
		}

		if (!descriptorSet->IsDomainGranted(capabilityMask))
		{
			return CapabilityNegotiationResult::Denied(CapabilityNegotiationDecision::DeniedDomainNotGranted);
		}

		return std::make_shared<CapabilityNegotiationResult>(CapabilityNegotiationDecision::Granted, std::make_shared<CapabilityGrant>(capabilityMask, scope, true, ownerDomainId, CapabilityDelegationPolicy::NonDelegable, CapabilityRevocationPolicy::RuntimeRevocable, CapabilityMigrationClass::DomainLocal, CapabilityEvidenceVisibility::HostOnly, CapabilityFrontendProjectionPolicy::ProjectIfCompatible));
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public CapabilityGrant NegotiateGrant(CapabilityDescriptorSet? descriptorSet, ulong capabilityMask, CapabilityGrantScope scope = CapabilityGrantScope.DomainGranted)
	std::shared_ptr<CapabilityGrant> CapabilityNegotiationService::NegotiateGrant(std::shared_ptr<CapabilityDescriptorSet> descriptorSet, unsigned long long capabilityMask, CapabilityGrantScope scope)
	{
		return Negotiate(descriptorSet, capabilityMask, scope)->Grant;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public CapabilityGrant NegotiateGrant(CapabilityDescriptorSet? descriptorSet, ulong capabilityMask, ulong ownerDomainId, CapabilityGrantScope scope = CapabilityGrantScope.DomainGranted)
	std::shared_ptr<CapabilityGrant> CapabilityNegotiationService::NegotiateGrant(std::shared_ptr<CapabilityDescriptorSet> descriptorSet, unsigned long long capabilityMask, unsigned long long ownerDomainId, CapabilityGrantScope scope)
	{
		return Negotiate(descriptorSet, capabilityMask, ownerDomainId, scope)->Grant;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public ulong ProjectEffectiveCompatibilityCaps(CapabilityDescriptorSet? descriptorSet)
	unsigned long long CapabilityNegotiationService::ProjectEffectiveCompatibilityCaps(std::shared_ptr<CapabilityDescriptorSet> descriptorSet)
	{
		return descriptorSet ? descriptorSet->getCompatibilityCapsProjection() : 0ULL;
	}
}
