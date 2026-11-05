#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class Lane7HandleNamespaceAuthority : unsigned char
	{
		Runtime = 0,
		Lane7AcceleratorDescriptor = 1,
		CompatibilityProjection = 2
	};

	enum class Lane7HandleVisibility : unsigned char
	{
		None = 0,
		VirtualHandles = 1,
		NativeBackendHandles = 2
	};


}
