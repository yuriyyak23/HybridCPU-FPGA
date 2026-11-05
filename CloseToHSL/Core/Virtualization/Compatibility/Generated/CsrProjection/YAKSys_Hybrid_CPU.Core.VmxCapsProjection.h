#pragma once

#include "../../../../Runtime/Capabilities/Publication/YAKSys_Hybrid_CPU.Core.CapabilityPublicationPolicy.h"
#include "../CapabilityProjection/YAKSys_Hybrid_CPU.Core.CapabilityDescriptorSetSchema.h"
#include "../../../../Runtime/Capabilities/Descriptors/YAKSys_Hybrid_CPU.Core.CapabilityDescriptorSet.h"
#include "VmxCapsProjection.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class VmxCapsProjection final : public std::enable_shared_from_this<VmxCapsProjection>
	{
	private:
		const std::shared_ptr<CapabilityPublicationPolicy> PublicationPolicy;
		const std::shared_ptr<CapabilityDescriptorSetSchema> Schema;
		const bool CompatibilityWriteFenceEnabled;

	public:
		VmxCapsProjection();

		VmxCapsProjection(std::shared_ptr<CapabilityPublicationPolicy> publicationPolicy);

		VmxCapsProjection(std::shared_ptr<CapabilityPublicationPolicy> publicationPolicy, std::shared_ptr<CapabilityDescriptorSetSchema> schema);

		VmxCapsProjection(std::shared_ptr<CapabilityPublicationPolicy> publicationPolicy, std::shared_ptr<CapabilityDescriptorSetSchema> schema, bool compatibilityWriteFenceEnabled);

		std::shared_ptr<CapabilityPublicationPolicy> getPublicationPolicy() const;

		std::shared_ptr<CapabilityDescriptorSetSchema> getSchema() const;

		bool getCompatibilityWriteFenceEnabled() const;

		unsigned long long Read(std::shared_ptr<CapabilityDescriptorSet> descriptorSet);

		bool CanPublishCapability(std::shared_ptr<CapabilityDescriptorSet> descriptorSet, unsigned long long capabilityMask);

		VmxCapsWriteResult EvaluateWrite(unsigned long long ignoredValue);
	};
}
