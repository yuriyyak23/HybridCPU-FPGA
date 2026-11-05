#pragma once

#include "YAKSys_Hybrid_CPU.Core.CapabilityGrant.h"
#include "CapabilityGrant.h"
#include "../../../Virtualization/Compatibility/Generated/CapabilityProjection/YAKSys_Hybrid_CPU.Core.CapabilityDescriptorSetSchema.h"
#include <vector>
#include <span>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class CapabilityGrantCollection final : public std::enable_shared_from_this<CapabilityGrantCollection>
	{
	private:

		const std::vector<std::shared_ptr<CapabilityGrant>> _grants;

	public:
		CapabilityGrantCollection();

		CapabilityGrantCollection(const std::vector<std::shared_ptr<CapabilityGrant>> &grants);

		static std::shared_ptr<CapabilityGrantCollection> getEmpty();

		std::span<const std::shared_ptr<CapabilityGrant>> getGrants() const;

		int getCount() const;

		unsigned long long ProjectMask(CapabilityGrantScope scope) const;

		unsigned long long getEffectiveCompatibilityMask() const;

		bool getHasAuthoritativeTypedGrants() const;

		bool TryGetGrant(unsigned long long capabilityMask, CapabilityGrantScope scope, std::shared_ptr<CapabilityGrant> &grant);

	private:
		static std::shared_ptr<CapabilityGrant> CreateInternalGrant(unsigned long long capabilityMask, CapabilityGrantScope scope, unsigned long long ownerDomainId);


	public:
		static std::shared_ptr<CapabilityGrantCollection> FromCompatibilityMasks(unsigned long long globalHardwareCaps, unsigned long long runtimeEnabledCaps, unsigned long long domainGrantedCaps, std::shared_ptr<CapabilityDescriptorSetSchema> schema, unsigned long long ownerDomainId = CapabilityGrant::DefaultRuntimeOwnerDomainId);
	};
}

