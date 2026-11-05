#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class CompletionRouteAuthority : unsigned char
	{
		Runtime = 0,
		DomainDescriptor = 1,
		CompatibilityProjection = 2
	};


}
