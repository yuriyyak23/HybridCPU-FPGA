#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class MemoryTranslationAuthorityViolation : unsigned char
	{
		None = 0,
		GuestCr3UsedAsAuthority = 1,
		VmcsIdentityUsedAsAuthority = 2,
		MissingGenericAddressSpaceRoot = 3,
		CompatibilityIdentityInAuthorityView = 4,
		MissingDomainControl = 5,
		DomainControlMismatch = 6
	};

// C# readonly record struct port. Kept as a value type to preserve the source contract.
	struct MemoryTranslationAuthorityRequest
	{
		bool UsesGuestCr3AsAuthority = false;
		bool UsesVmcsIdentityAsAuthority = false;
		bool HasGenericAddressSpaceRoot = false;
		bool AuthorityViewIncludesCompatibilityIdentity = false;
	};


}
