#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [System.Flags] public enum FenceOrderingScope : ushort
	enum class FenceOrderingScope : int
	{
		None = 0,
		Cpu = 1 << 0,
		Dma = 1 << 1,
		Iommu = 1 << 2,
		Lane6 = 1 << 3,
		Lane7 = 1 << 4,
		MemoryVisibility = 1 << 5,
		CompletionPublication = 1 << 6,
		All = Cpu | Dma | Iommu | Lane6 | Lane7 | MemoryVisibility | CompletionPublication
	};

	enum class FenceDomainAuthority : unsigned char
	{
		Runtime = 0,
		LaneDescriptor = 1,
		CompatibilityProjection = 2
	};


}
