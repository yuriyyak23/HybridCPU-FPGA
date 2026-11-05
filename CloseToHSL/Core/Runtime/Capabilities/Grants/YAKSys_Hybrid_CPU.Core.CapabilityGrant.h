#pragma once

#include "CapabilityGrant.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class CapabilityGrant final : public std::enable_shared_from_this<CapabilityGrant>
	{
	private:
		static inline const std::shared_ptr<CapabilityGrant> Denied = std::make_shared<CapabilityGrant>();
		const unsigned long long CapabilityMask;
		const CapabilityGrantScope Scope;
		const bool IsGranted;
		const unsigned long long OwnerDomainId;
		const CapabilityDelegationPolicy DelegationPolicy;
		const CapabilityRevocationPolicy RevocationPolicy;
		const CapabilityMigrationClass MigrationClass;
		const CapabilityEvidenceVisibility EvidenceVisibility;
		const CapabilityFrontendProjectionPolicy FrontendProjectionPolicy;

	public:
		static constexpr unsigned long long NoOwnerDomainId = 0;
		static constexpr unsigned long long DefaultRuntimeOwnerDomainId = 1;

		CapabilityGrant();

		CapabilityGrant(unsigned long long capabilityMask, CapabilityGrantScope scope, bool isGranted);

		CapabilityGrant(unsigned long long capabilityMask, CapabilityGrantScope scope, bool isGranted, unsigned long long ownerDomainId, CapabilityDelegationPolicy delegationPolicy, CapabilityRevocationPolicy revocationPolicy, CapabilityMigrationClass migrationClass, CapabilityEvidenceVisibility evidenceVisibility, CapabilityFrontendProjectionPolicy frontendProjectionPolicy);

		static std::shared_ptr<CapabilityGrant> getDenied();

		unsigned long long getCapabilityMask() const;

		CapabilityGrantScope getScope() const;

		bool isGranted() const;

		unsigned long long getOwnerDomainId() const;

		CapabilityDelegationPolicy getDelegationPolicy() const;

		CapabilityRevocationPolicy getRevocationPolicy() const;

		CapabilityMigrationClass getMigrationClass() const;

		CapabilityEvidenceVisibility getEvidenceVisibility() const;

		CapabilityFrontendProjectionPolicy getFrontendProjectionPolicy() const;

		bool getHasTypedAuthority() const;

		bool Grants(unsigned long long capabilityMask);

		bool isPublishableCompatibilityGrant() const;
	};
}
