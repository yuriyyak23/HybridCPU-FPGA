#pragma once

#include "../Grants/YAKSys_Hybrid_CPU.Core.CapabilityGrantCollection.h"
#include "../Grants/YAKSys_Hybrid_CPU.Core.CapabilityGrant.h"
#include "../Grants/CapabilityGrant.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class CapabilityDescriptorSet final : public std::enable_shared_from_this<CapabilityDescriptorSet>
	{
	private:
		const std::shared_ptr<CapabilityGrantCollection> TypedGrants;

	public:
		CapabilityDescriptorSet();

		CapabilityDescriptorSet(std::shared_ptr<CapabilityGrantCollection> typedGrants);

		static std::shared_ptr<CapabilityDescriptorSet> getEmpty();

		unsigned long long getGlobalHardwareCaps() const;

		unsigned long long getRuntimeEnabledCaps() const;

		unsigned long long getDomainGrantedCaps() const;

		std::shared_ptr<CapabilityGrantCollection> getTypedGrants() const;

		unsigned long long getCompatibilityCapsProjection() const;

		unsigned long long getEffectiveCaps() const;

		bool HasHardwareCapability(unsigned long long capabilityMask);

		bool IsRuntimeEnabled(unsigned long long capabilityMask);

		bool IsDomainGranted(unsigned long long capabilityMask);

		bool HasEffectiveCapability(unsigned long long capabilityMask);

		std::shared_ptr<CapabilityGrant> CreateGrant(unsigned long long capabilityMask, CapabilityGrantScope scope);

		std::shared_ptr<CapabilityGrant> CreateGrant(unsigned long long capabilityMask, CapabilityGrantScope scope, unsigned long long ownerDomainId, CapabilityDelegationPolicy delegationPolicy = CapabilityDelegationPolicy::NonDelegable, CapabilityRevocationPolicy revocationPolicy = CapabilityRevocationPolicy::RuntimeRevocable, CapabilityMigrationClass migrationClass = CapabilityMigrationClass::DomainLocal, CapabilityEvidenceVisibility evidenceVisibility = CapabilityEvidenceVisibility::HostOnly, CapabilityFrontendProjectionPolicy frontendProjectionPolicy = CapabilityFrontendProjectionPolicy::ProjectIfCompatible);

	private:
		bool HasTypedGrant(unsigned long long capabilityMask, CapabilityGrantScope scope);


	public:
		CapabilityDescriptorSet(unsigned long long globalHardwareCaps, unsigned long long runtimeEnabledCaps, unsigned long long domainGrantedCaps);

		std::shared_ptr<CapabilityDescriptorSet> WithRuntimeEnabledCaps(unsigned long long runtimeEnabledCaps);

		std::shared_ptr<CapabilityDescriptorSet> WithDomainGrantedCaps(unsigned long long domainGrantedCaps);
	};
}
