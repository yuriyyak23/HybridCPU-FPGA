#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class Lane6TokenNamespaceAuthority : unsigned char
	{
		Runtime = 0,
		Lane6DomainDescriptor = 1,
		CompatibilityProjection = 2
	};

	enum class Lane6TokenVisibility : unsigned char
	{
		None = 0,
		VirtualTokens = 1,
		NativeTokens = 2
	};


}
