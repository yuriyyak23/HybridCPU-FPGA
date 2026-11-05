#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class SecureHostInspectionMode : unsigned char
	{
		Denied = 0,
		MetadataOnly = 1,
		ExplicitSharedOnly = 2,
		DebugOnly = 3
	};


}
