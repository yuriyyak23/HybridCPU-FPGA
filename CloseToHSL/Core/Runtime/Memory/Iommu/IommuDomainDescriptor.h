#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class IommuDomainAuthority : unsigned char
	{
		Runtime = 0,
		IoDomainDescriptor = 1,
		CompatibilityProjection = 2
	};


}
