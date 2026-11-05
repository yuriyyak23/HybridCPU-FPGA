#include "YAKSys_Hybrid_CPU.Core.VmxCapsProjection.h"

namespace YAKSys_Hybrid_CPU::Core
{

	VmxCapsProjection::VmxCapsProjection() : VmxCapsProjection(CapabilityPublicationPolicy::getReadOnlyCompatibilityAlias(), CapabilityDescriptorSetSchema::getVmxCompatibility())
	{
	}

	VmxCapsProjection::VmxCapsProjection(std::shared_ptr<CapabilityPublicationPolicy> publicationPolicy) : VmxCapsProjection(publicationPolicy, CapabilityDescriptorSetSchema::getVmxCompatibility())
	{
	}

	VmxCapsProjection::VmxCapsProjection(std::shared_ptr<CapabilityPublicationPolicy> publicationPolicy, std::shared_ptr<CapabilityDescriptorSetSchema> schema) : VmxCapsProjection(publicationPolicy, schema, false)
	{
	}

	VmxCapsProjection::VmxCapsProjection(std::shared_ptr<CapabilityPublicationPolicy> publicationPolicy, std::shared_ptr<CapabilityDescriptorSetSchema> schema, bool compatibilityWriteFenceEnabled)
		: PublicationPolicy((publicationPolicy != nullptr) ? publicationPolicy : CapabilityPublicationPolicy::getFailClosed()), Schema((schema != nullptr) ? schema : std::make_shared<CapabilityDescriptorSetSchema>()), CompatibilityWriteFenceEnabled(compatibilityWriteFenceEnabled)
	{
	}

	std::shared_ptr<CapabilityPublicationPolicy> VmxCapsProjection::getPublicationPolicy() const
	{
		return PublicationPolicy;
	}

	std::shared_ptr<CapabilityDescriptorSetSchema> VmxCapsProjection::getSchema() const
	{
		return Schema;
	}

	bool VmxCapsProjection::getCompatibilityWriteFenceEnabled() const
	{
		return CompatibilityWriteFenceEnabled;
	}

	unsigned long long VmxCapsProjection::Read(std::shared_ptr<CapabilityDescriptorSet> descriptorSet)
	{
		return getSchema()->FilterCompatibilityCaps(getPublicationPolicy()->ProjectCompatibilityAlias(descriptorSet));
	}

	bool VmxCapsProjection::CanPublishCapability(std::shared_ptr<CapabilityDescriptorSet> descriptorSet, unsigned long long capabilityMask)
	{
		return getSchema()->ContainsOnlyKnownCompatibilityCaps(capabilityMask) && getPublicationPolicy()->CanPublishCapability(descriptorSet, capabilityMask);
	}

	VmxCapsWriteResult VmxCapsProjection::EvaluateWrite(unsigned long long ignoredValue)
	{
		return getPublicationPolicy()->ShouldRejectCompatibilityWrite() || !getCompatibilityWriteFenceEnabled() ? VmxCapsWriteResult::Rejected : VmxCapsWriteResult::CompatibilityNoEffect;
	}
}
