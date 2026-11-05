#include "YAKSys_Hybrid_CPU.Core.CapabilityGrantAuthorityContract.h"
#include "../../../Runtime/Capabilities/Grants/CapabilityGrant.h"

namespace YAKSys_Hybrid_CPU::Core
{

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public CapabilityGrantAuthorityViolation Evaluate(CapabilityGrant? grant)
	CapabilityGrantAuthorityViolation CapabilityGrantAuthorityContract::Evaluate(std::shared_ptr<CapabilityGrant> grant)
	{
		if (grant == nullptr)
		{
			return CapabilityGrantAuthorityViolation::MissingGrant;
		}

		if (!grant->isGranted())
		{
			return CapabilityGrantAuthorityViolation::None;
		}

		if (grant->getCapabilityMask() == 0)
		{
			return CapabilityGrantAuthorityViolation::EmptyGrantedMask;
		}

		if (!grant->getHasTypedAuthority())
		{
			return CapabilityGrantAuthorityViolation::MissingTypedAuthority;
		}

		if (grant->getOwnerDomainId() == 0 && grant->getScope() == CapabilityGrantScope::DomainGranted)
		{
			return CapabilityGrantAuthorityViolation::MissingOwnerDomain;
		}

		if (grant->getDelegationPolicy() == CapabilityDelegationPolicy::None)
		{
			return CapabilityGrantAuthorityViolation::DelegationPolicyMissing;
		}

		if (grant->getRevocationPolicy() == CapabilityRevocationPolicy::None)
		{
			return CapabilityGrantAuthorityViolation::RevocationPolicyMissing;
		}

		if (grant->getMigrationClass() == CapabilityMigrationClass::None)
		{
			return CapabilityGrantAuthorityViolation::MigrationClassMissing;
		}

		if (grant->getEvidenceVisibility() == CapabilityEvidenceVisibility::None)
		{
			return CapabilityGrantAuthorityViolation::EvidenceVisibilityMissing;
		}

		if (grant->getFrontendProjectionPolicy() == CapabilityFrontendProjectionPolicy::None)
		{
			return CapabilityGrantAuthorityViolation::FrontendProjectionPolicyMissing;
		}

		if (grant->getScope() == CapabilityGrantScope::CompatibilityProjection && !grant->isPublishableCompatibilityGrant())
		{
			return CapabilityGrantAuthorityViolation::CompatibilityProjectionPolicyDenied;
		}

		return CapabilityGrantAuthorityViolation::None;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public bool IsSatisfied(CapabilityGrant? grant)
	bool CapabilityGrantAuthorityContract::IsSatisfied(std::shared_ptr<CapabilityGrant> grant)
	{
		return Evaluate(grant) == CapabilityGrantAuthorityViolation::None;
	}
}
