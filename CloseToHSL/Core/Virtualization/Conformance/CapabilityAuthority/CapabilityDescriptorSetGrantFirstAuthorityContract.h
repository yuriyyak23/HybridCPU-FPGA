#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class CapabilityDescriptorSetGrantFirstAuthorityViolation : unsigned char
	{
		None = 0,
		MissingDescriptorSet = 1,
		HardwareProjectionBypassesTypedGrants = 2,
		RuntimeProjectionBypassesTypedGrants = 3,
		DomainProjectionBypassesTypedGrants = 4,
		CompatibilityProjectionBypassesTypedGrants = 5,
		EffectiveCapabilityBypassesTypedGrant = 6,
		CompatibilitySchemaClaimsDescriptorSetAuthority = 7
	};


}
