#pragma once

namespace YAKSys_Hybrid_CPU::Core
{
// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [System.Flags] public enum TrapPolicyClass : ulong
	enum class TrapPolicyClass : int
	{
		None = 0,
		Instruction = 1ULL << 0,
		Csr = 1ULL << 1,
		Memory = 1ULL << 2,
		CompatibilityOperation = 1ULL << 3,
		LaneOperation = 1ULL << 4
	};

	constexpr TrapPolicyClass operator&(TrapPolicyClass left, TrapPolicyClass right) noexcept
	{
		return static_cast<TrapPolicyClass>(static_cast<int>(left) & static_cast<int>(right));
	}

}
