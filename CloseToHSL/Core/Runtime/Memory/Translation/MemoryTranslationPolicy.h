#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class MemoryTranslationAuthority : unsigned char
	{
		RuntimeOwned = 0,
		CompatibilityProjection = 1
	};

// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [System.Flags] public enum TranslationInvalidationPermission : byte
	enum class TranslationInvalidationPermission : unsigned char
	{
		None = 0,
		Address = 1 << 0,
		AddressSpace = 1 << 1,
		Global = 1 << 2
	};


}
