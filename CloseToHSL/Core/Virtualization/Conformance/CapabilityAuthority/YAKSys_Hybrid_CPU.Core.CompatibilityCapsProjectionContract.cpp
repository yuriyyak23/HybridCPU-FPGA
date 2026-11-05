#include "YAKSys_Hybrid_CPU.Core.CompatibilityCapsProjectionContract.h"
#include "../../Compatibility/Generated/CapabilityProjection/YAKSys_Hybrid_CPU.Core.CapabilityDescriptorSetSchema.h"

namespace YAKSys_Hybrid_CPU::Core
{

	CompatibilityCapsProjectionViolation CompatibilityCapsProjectionContract::Evaluate(std::shared_ptr<CapabilityDescriptorSet> descriptorSet, unsigned long long projectedCompatibilityCaps)
	{
		if (descriptorSet == nullptr)
		{
			return CompatibilityCapsProjectionViolation::MissingDescriptorSet;
		}

		unsigned long long expectedProjection = descriptorSet->getCompatibilityCapsProjection() & CapabilityDescriptorSetSchema::getVmxCompatibility()->getCompatibilityPublicationMask();

		if (projectedCompatibilityCaps != expectedProjection)
		{
			return CompatibilityCapsProjectionViolation::ProjectionMismatch;
		}

		if (expectedProjection != descriptorSet->getTypedGrants()->getEffectiveCompatibilityMask())
		{
			return CompatibilityCapsProjectionViolation::ProjectionBypassesTypedGrants;
		}

		return CompatibilityCapsProjectionViolation::None;
	}

	bool CompatibilityCapsProjectionContract::IsSatisfied(std::shared_ptr<CapabilityDescriptorSet> descriptorSet, unsigned long long projectedCompatibilityCaps)
	{
		return Evaluate(descriptorSet, projectedCompatibilityCaps) == CompatibilityCapsProjectionViolation::None;
	}
}
