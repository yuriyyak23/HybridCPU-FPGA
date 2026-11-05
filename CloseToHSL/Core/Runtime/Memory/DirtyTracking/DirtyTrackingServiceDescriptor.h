#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class DirtyTrackingAuthority : unsigned char
	{
		RuntimeOwned = 0,
		CompatibilityProjection = 1
	};

	enum class DirtyTrackingVisibility : unsigned char
	{
		HostOwnedOnly = 0,
		GuestVisibleSummary = 1
	};


}
