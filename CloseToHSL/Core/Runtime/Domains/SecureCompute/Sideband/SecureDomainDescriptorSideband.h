#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class SecureSidebandVisibility : unsigned char
	{
		InternalOnly = 0,
		GuestVisible = 1,
		CompatibilityAlias = 2
	};


}
