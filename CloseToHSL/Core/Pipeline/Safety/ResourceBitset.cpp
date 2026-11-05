#include "ResourceBitset.h"

namespace YAKSys_Hybrid_CPU::Core
{

	ResourceBitset::ResourceBitset(unsigned long long low, unsigned long long high)
	{
		Low = low;
		High = high;
	}

	ResourceBitset::ResourceBitset(unsigned long long mask)
	{
		Low = mask;
		High = 0;
	}

	bool ResourceBitset::isZero() const
	{
		return Low == 0 && High == 0;
	}

	bool ResourceBitset::isNonZero() const
	{
		return Low != 0 || High != 0;
	}

	ResourceBitset ResourceBitset::operator | (ResourceBitset b)
	{
		return ResourceBitset(this->Low | b.Low, this->High | b.High);
	}

	ResourceBitset ResourceBitset::operator &(ResourceBitset b)
	{
		return ResourceBitset(this->Low & b.Low, this->High & b.High);
	}

	ResourceBitset ResourceBitset::operator ~()
	{
		return ResourceBitset(~this->Low, ~this->High);
	}

	bool ResourceBitset::operator == (ResourceBitset b)
	{
		return this->Low == b.Low && this->High == b.High;
	}

	bool ResourceBitset::operator != (ResourceBitset b)
	{
		return !(*this == b);
	}


	ResourceBitset::operator unsigned long long()
	{
		return this->Low;
	}

	bool ResourceBitset::ConflictsWith(ResourceBitset other) const
	{
		// Non-read resources (WAW, domains, LSU, DMA, etc.) overlap directly
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

	bool ResourceBitset::NoConflictsWith(ResourceBitset other) const
	{
		return !ConflictsWith(other);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public override readonly bool Equals(Object? obj)
	bool ResourceBitset::Equals(std::any obj) const
	{
		const auto *other = std::any_cast<ResourceBitset>(&obj);
		return other != nullptr && Equals(*other);
	}

	bool ResourceBitset::Equals(ResourceBitset other) const
	{
		return Low == other.Low && High == other.High;
	}

	int ResourceBitset::GetHashCode() const
	{
		return static_cast<int>((Low ^ (Low >> 32) ^ High ^ (High >> 32)) & 0x7FFFFFFFULL);
	}

	std::wstring ResourceBitset::ToString() const
	{
		return std::format(L"ResourceBitset(Low=0x{:016X}, High=0x{:016X})", Low, High);
	}

	unsigned int ResourceBitset::GetBank(int bankIndex) const
	{
		unsigned int temp_var131;
		switch (bankIndex)
		{
			case 0:
				temp_var131 = static_cast<unsigned int>(Low & 0xFFFFFFFFULL);
				break;
			case 1:
				temp_var131 = static_cast<unsigned int>(Low >> 32);
				break;
			case 2:
				temp_var131 = static_cast<unsigned int>(High & 0xFFFFFFFFULL);
				break;
			case 3:
				temp_var131 = static_cast<unsigned int>(High >> 32);
				break;
			default:
				temp_var131 = 0;
				break;
		}
		return temp_var131;
	}

	unsigned char ResourceBitset::ActiveBanks() const
	{
		unsigned char banks = 0;
		if ((Low & 0x00000000FFFFFFFFULL) != 0)
		{
			banks |= 0x01;
		}
		if ((Low & 0xFFFFFFFF00000000ULL) != 0)
		{
			banks |= 0x02;
		}
		if ((High & 0x00000000FFFFFFFFULL) != 0)
		{
			banks |= 0x04;
		}
		if ((High & 0xFFFFFFFF00000000ULL) != 0)
		{
			banks |= 0x08;
		}
		return banks;
	}
}
