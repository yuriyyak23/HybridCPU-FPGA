#include "SafetyMask128.h"

namespace YAKSys_Hybrid_CPU::Core
{

	SafetyMask128::SafetyMask128(unsigned long long low, unsigned long long high)
	{
		Low = low;
		High = high;
	}

	SafetyMask128::SafetyMask128(unsigned long long mask)
	{
		Low = mask;
		High = 0;
	}

	bool SafetyMask128::isZero() const
	{
		return Low == 0 && High == 0;
	}

	bool SafetyMask128::isNonZero() const
	{
		return Low != 0 || High != 0;
	}

	SafetyMask128 SafetyMask128::operator | (SafetyMask128 b) const
	{
		return SafetyMask128(this->Low | b.Low, this->High | b.High);
	}

	SafetyMask128 SafetyMask128::operator &(SafetyMask128 b) const
	{
		return SafetyMask128(this->Low & b.Low, this->High & b.High);
	}

	bool SafetyMask128::operator == (SafetyMask128 b) const
	{
		return this->Low == b.Low && this->High == b.High;
	}

	bool SafetyMask128::operator != (SafetyMask128 b) const
	{
		return !(*this == b);
	}


	bool SafetyMask128::ConflictsWith(SafetyMask128 other) const
	{
		// Non-read resources (WAW, domains, LSU, etc.) overlap directly
		unsigned long long nonReadLowA = Low & ~0xFFFFULL;
		unsigned long long nonReadLowB = other.Low & ~0xFFFFULL;

		if ((nonReadLowA & nonReadLowB) != 0)
		{
			return true;
		}
		if ((High & other.High) != 0)
		{
			return true;
		}

		// Cross-dependencies for Registers: Read vs Write (RAW / WAR)
		unsigned long long myReads = Low & 0xFFFFULL;
		unsigned long long theirWrites = (other.Low >> 16) & 0xFFFFULL;
		unsigned long long myWrites = (Low >> 16) & 0xFFFFULL;
		unsigned long long theirReads = other.Low & 0xFFFFULL;

		if ((myReads & theirWrites) != 0)
		{
			return true;
		}
		if ((myWrites & theirReads) != 0)
		{
			return true;
		}

		return false;
	}

	bool SafetyMask128::NoConflictsWith(SafetyMask128 other) const
	{
		return !ConflictsWith(other);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public override bool Equals(Object? obj)
	bool SafetyMask128::Equals(std::any obj) const
	{
		const auto *other = std::any_cast<SafetyMask128>(&obj);
		return other != nullptr && Equals(*other);
	}

	bool SafetyMask128::Equals(SafetyMask128 other) const
	{
		return Low == other.Low && High == other.High;
	}

	int SafetyMask128::GetHashCode() const
	{
		return static_cast<int>((Low ^ (Low >> 32) ^ High ^ (High >> 32)) & 0x7FFFFFFFULL);
	}

	std::wstring SafetyMask128::ToString() const
	{
		return std::format(L"SafetyMask128(Low=0x{:016X}, High=0x{:016X})", Low, High);
	}
}
