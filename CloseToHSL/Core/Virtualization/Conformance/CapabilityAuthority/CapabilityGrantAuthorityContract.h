#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class CapabilityGrantAuthorityViolation : unsigned char
	{
		None = 0,
		MissingGrant = 1,
		EmptyGrantedMask = 2,
		MissingTypedAuthority = 3,
		MissingOwnerDomain = 4,
		DelegationPolicyMissing = 5,
		RevocationPolicyMissing = 6,
		MigrationClassMissing = 7,
		EvidenceVisibilityMissing = 8,
		FrontendProjectionPolicyMissing = 9,
		CompatibilityProjectionPolicyDenied = 10
	};


}
