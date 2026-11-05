#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class CapabilityCallerMaskIngressAuthorityViolation : unsigned char
	{
		None = 0,
		NonTypedRequirementAccepted = 1,
		MissingTypedProjectionGrant = 2,
		CompatibilityAliasPublishedAsAuthority = 3,
		NestedCompatibilityAliasPublishedAsAuthority = 4,
		NestedCompatibilityMaskProofAccepted = 5,
		HostOwnedEvidenceExposedToCompatibilityProjection = 6
	};


}
