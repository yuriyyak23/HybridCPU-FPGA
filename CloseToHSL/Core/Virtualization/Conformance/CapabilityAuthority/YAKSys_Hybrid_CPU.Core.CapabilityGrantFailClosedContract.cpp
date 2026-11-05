#include "YAKSys_Hybrid_CPU.Core.CapabilityGrantFailClosedContract.h"
#include "../../../Runtime/Capabilities/Grants/YAKSys_Hybrid_CPU.Core.CapabilityGrant.h"
#include "../../../Runtime/Capabilities/Grants/CapabilityGrant.h"

namespace YAKSys_Hybrid_CPU::Core
{

	CapabilityGrantFailClosedViolation CapabilityGrantFailClosedContract::Evaluate(std::shared_ptr<CapabilityDescriptorSet> descriptorSet, unsigned long long unmappedCapabilityMask)
	{
		if (descriptorSet == nullptr)
		{
			return CapabilityGrantFailClosedViolation::MissingDescriptorSet;
		}

		std::shared_ptr<CapabilityGrant> grant = descriptorSet->CreateGrant(unmappedCapabilityMask, CapabilityGrantScope::DomainGranted);

		if (grant->isGranted())
		{
			return CapabilityGrantFailClosedViolation::MissingTypedGrantWasGranted;
		}

		std::shared_ptr<CapabilityGrant> ownerAwareGrant = descriptorSet->CreateGrant(unmappedCapabilityMask, CapabilityGrantScope::DomainGranted, CapabilityGrant::DefaultRuntimeOwnerDomainId);

		return ownerAwareGrant->isGranted() ? CapabilityGrantFailClosedViolation::OwnerAwareMissingGrantWasGranted : CapabilityGrantFailClosedViolation::None;
	}

	bool CapabilityGrantFailClosedContract::IsSatisfied(std::shared_ptr<CapabilityDescriptorSet> descriptorSet, unsigned long long unmappedCapabilityMask)
	{
		return Evaluate(descriptorSet, unmappedCapabilityMask) == CapabilityGrantFailClosedViolation::None;
	}
}
