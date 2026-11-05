#include "YAKSys_Hybrid_CPU.Core.CapabilityGrantCollectionAuthorityContract.h"
#include "../../../Runtime/Capabilities/Grants/YAKSys_Hybrid_CPU.Core.CapabilityGrantCollection.h"
#include "../../Compatibility/Generated/CapabilityProjection/YAKSys_Hybrid_CPU.Core.CapabilityDescriptorSetSchema.h"

namespace YAKSys_Hybrid_CPU::Core
{

	CapabilityGrantCollectionAuthorityViolation CapabilityGrantCollectionAuthorityContract::Evaluate(std::shared_ptr<CapabilityDescriptorSet> descriptorSet)
	{
		if (descriptorSet == nullptr)
		{
			return CapabilityGrantCollectionAuthorityViolation::MissingCollection;
		}

		std::shared_ptr<CapabilityGrantCollection> collection = descriptorSet->getTypedGrants();
		unsigned long long projectedEffectiveMask = descriptorSet->getCompatibilityCapsProjection() & CapabilityDescriptorSetSchema::getVmxCompatibility()->getCompatibilityPublicationMask();

		if (projectedEffectiveMask != 0 && collection->getCount() == 0)
		{
			return CapabilityGrantCollectionAuthorityViolation::EmptyCollectionForEffectiveCaps;
		}

		for (auto grant : collection->getGrants())
		{
			if (!_grantAuthority->IsSatisfied(grant))
			{
				return CapabilityGrantCollectionAuthorityViolation::MissingTypedAuthority;
			}
		}

		return collection->getEffectiveCompatibilityMask() == projectedEffectiveMask ? CapabilityGrantCollectionAuthorityViolation::None : CapabilityGrantCollectionAuthorityViolation::EffectiveMaskMismatch;
	}

	bool CapabilityGrantCollectionAuthorityContract::IsSatisfied(std::shared_ptr<CapabilityDescriptorSet> descriptorSet)
	{
		return Evaluate(descriptorSet) == CapabilityGrantCollectionAuthorityViolation::None;
	}
}
