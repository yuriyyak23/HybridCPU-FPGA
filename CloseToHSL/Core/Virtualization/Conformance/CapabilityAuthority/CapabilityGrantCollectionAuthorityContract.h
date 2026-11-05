#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class CapabilityGrantCollectionAuthorityViolation : unsigned char
	{
		None = 0,
		MissingCollection = 1,
		EmptyCollectionForEffectiveCaps = 2,
		MissingTypedAuthority = 3,
		EffectiveMaskMismatch = 4
	};


}
