#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class Lane7TokenNamespaceAuthority : unsigned char
	{
		Runtime = 0,
		Lane7AcceleratorDescriptor = 1,
		CompatibilityProjection = 2
	};

	enum class Lane7TokenVisibility : unsigned char
	{
		None = 0,
		VirtualTokens = 1,
		NativeTokens = 2
	};


}
