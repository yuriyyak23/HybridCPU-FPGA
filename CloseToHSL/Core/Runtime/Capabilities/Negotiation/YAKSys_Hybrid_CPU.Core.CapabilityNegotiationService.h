#pragma once

#include "CapabilityNegotiationService.h"

#include "../Descriptors/YAKSys_Hybrid_CPU.Core.CapabilityDescriptorSet.h"
#include "../Grants/CapabilityGrant.h"
#include "../Grants/YAKSys_Hybrid_CPU.Core.CapabilityGrant.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class CapabilityNegotiationService final : public std::enable_shared_from_this<CapabilityNegotiationService>
	{
	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public CapabilityNegotiationResult Negotiate(CapabilityDescriptorSet? descriptorSet, ulong capabilityMask, CapabilityGrantScope scope = CapabilityGrantScope.DomainGranted)
		std::shared_ptr<CapabilityNegotiationResult> Negotiate(std::shared_ptr<CapabilityDescriptorSet> descriptorSet, unsigned long long capabilityMask, CapabilityGrantScope scope = CapabilityGrantScope::DomainGranted);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public CapabilityNegotiationResult Negotiate(CapabilityDescriptorSet? descriptorSet, ulong capabilityMask, ulong ownerDomainId, CapabilityGrantScope scope = CapabilityGrantScope.DomainGranted)
		std::shared_ptr<CapabilityNegotiationResult> Negotiate(std::shared_ptr<CapabilityDescriptorSet> descriptorSet, unsigned long long capabilityMask, unsigned long long ownerDomainId, CapabilityGrantScope scope = CapabilityGrantScope::DomainGranted);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public CapabilityGrant NegotiateGrant(CapabilityDescriptorSet? descriptorSet, ulong capabilityMask, CapabilityGrantScope scope = CapabilityGrantScope.DomainGranted)
		std::shared_ptr<CapabilityGrant> NegotiateGrant(std::shared_ptr<CapabilityDescriptorSet> descriptorSet, unsigned long long capabilityMask, CapabilityGrantScope scope = CapabilityGrantScope::DomainGranted);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public CapabilityGrant NegotiateGrant(CapabilityDescriptorSet? descriptorSet, ulong capabilityMask, ulong ownerDomainId, CapabilityGrantScope scope = CapabilityGrantScope.DomainGranted)
		std::shared_ptr<CapabilityGrant> NegotiateGrant(std::shared_ptr<CapabilityDescriptorSet> descriptorSet, unsigned long long capabilityMask, unsigned long long ownerDomainId, CapabilityGrantScope scope = CapabilityGrantScope::DomainGranted);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public ulong ProjectEffectiveCompatibilityCaps(CapabilityDescriptorSet? descriptorSet)
		unsigned long long ProjectEffectiveCompatibilityCaps(std::shared_ptr<CapabilityDescriptorSet> descriptorSet);
	};
}
