#pragma once
#include <limits>

namespace YAKSys_Hybrid_CPU::Core
{
	enum class TrapTargetKind : unsigned char
	{
		None = 0,
		InstructionOpcode = 1,
		CsrAddress = 2,
		MemoryRange = 3,
		CompatibilityOperation = 4,
		LaneOperation = 5,
		VirtualTimer = 6,
		PreemptionTimer = 7
	};

	enum class TrapAccessType : unsigned char
	{
		None = 0,
		Read = 1,
		Write = 2,
		Execute = 3,
		CompatibilityOperation = 4,
		LaneOperation = 5,
		TimerExpiry = 6
	};

// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [System.Flags] public enum TrapAccessMask : byte
	enum class TrapAccessMask : unsigned char
	{
		None = 0,
		Read = 1 << 0,
		Write = 1 << 1,
		Execute = 1 << 2,
		CompatibilityOperation = 1 << 3,
		LaneOperation = 1 << 4,
		TimerExpiry = 1 << 5,
		All = Read | Write | Execute | CompatibilityOperation | LaneOperation | TimerExpiry
	};

struct MemoryTrapRange {
    const unsigned long long BaseAddress, Length; const TrapAccessMask AccessMask; const bool IsValid;
    MemoryTrapRange(unsigned long long base, unsigned long long length, TrapAccessMask access) : BaseAddress(base), Length(length), AccessMask(access), IsValid(length != 0 && access != TrapAccessMask::None) {}
    static unsigned long long SaturatingEnd(unsigned long long address, unsigned long long length) { if (!length) length=1; auto end=address+length; return end < address ? (std::numeric_limits<unsigned long long>::max)() : end; }
    static TrapAccessMask ToMask(TrapAccessType access) { switch (access) { case TrapAccessType::Read: return TrapAccessMask::Read; case TrapAccessType::Write: return TrapAccessMask::Write; case TrapAccessType::Execute: return TrapAccessMask::Execute; case TrapAccessType::CompatibilityOperation: return TrapAccessMask::CompatibilityOperation; case TrapAccessType::LaneOperation: return TrapAccessMask::LaneOperation; case TrapAccessType::TimerExpiry: return TrapAccessMask::TimerExpiry; default: return TrapAccessMask::None; } }
    bool Matches(unsigned long long address, unsigned long long length, TrapAccessType access) const {
        if (!IsValid || (static_cast<unsigned char>(AccessMask) & static_cast<unsigned char>(ToMask(access))) == 0) return false;
        return address < SaturatingEnd(BaseAddress, Length) && SaturatingEnd(address, length) > BaseAddress;
    }
};
}