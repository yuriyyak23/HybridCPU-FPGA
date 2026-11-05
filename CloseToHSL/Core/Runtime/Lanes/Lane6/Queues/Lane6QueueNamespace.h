#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class Lane6QueueNamespaceAuthority : unsigned char
	{
		Runtime = 0,
		Lane6DomainDescriptor = 1,
		CompatibilityProjection = 2
	};

	enum class Lane6QueueVisibility : unsigned char
	{
		None = 0,
		VirtualQueueIds = 1,
		NativeQueueHandles = 2
	};


}
