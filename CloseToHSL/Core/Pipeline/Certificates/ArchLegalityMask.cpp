#include "ArchLegalityMask.h"

namespace YAKSys_Hybrid_CPU::Core
{

	ArchLegalityMask::ArchLegalityMask(SafetyMask128 mask) : Mask(mask)
	{
	}

	ArchLegalityMask::ArchLegalityMask(unsigned long long low, unsigned long long high) : Mask(SafetyMask128(low, high))
	{
	}

	bool ArchLegalityMask::isZero() const
	{
		return Mask.isZero();
	}

	bool ArchLegalityMask::ConflictsWith(ArchLegalityMask other) const
	{
		return Mask.ConflictsWith(other.Mask);
	}

	ArchLegalityMask ArchLegalityMask::operator | (ArchLegalityMask b)
	{
		return ArchLegalityMask(this->Mask | b.Mask);
	}

	ArchLegalityMask ArchLegalityMask::operator &(ArchLegalityMask b)
	{
		return ArchLegalityMask(this->Mask & b.Mask);
	}

	ArchLegalityMask::operator SafetyMask128()
	{
		return this->Mask;
	}


	bool ArchLegalityMask::Equals(ArchLegalityMask other) const
	{
		return Mask.Equals(other.Mask);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public override bool Equals(Object? obj)
	bool ArchLegalityMask::Equals(std::any obj) const
	{
		const auto *m = std::any_cast<ArchLegalityMask>(&obj); return m != nullptr && Equals(*m);
	}

	int ArchLegalityMask::GetHashCode() const
	{
		return Mask.GetHashCode();
	}

	std::wstring ArchLegalityMask::ToString() const
	{
		return std::format(L"ArchLegalityMask({})", Mask.ToString());
	}

	BackendHazardMask::BackendHazardMask(SafetyMask128 mask) : Mask(mask)
	{
	}

	BackendHazardMask::BackendHazardMask(unsigned long long low, unsigned long long high) : Mask(SafetyMask128(low, high))
	{
	}

	bool BackendHazardMask::isZero() const
	{
		return Mask.isZero();
	}

	bool BackendHazardMask::ConflictsWith(BackendHazardMask other) const
	{
		return Mask.ConflictsWith(other.Mask);
	}

	BackendHazardMask BackendHazardMask::operator | (BackendHazardMask b)
	{
		return BackendHazardMask(this->Mask | b.Mask);
	}

	BackendHazardMask BackendHazardMask::operator &(BackendHazardMask b)
	{
		return BackendHazardMask(this->Mask & b.Mask);
	}

	BackendHazardMask::operator SafetyMask128()
	{
		return this->Mask;
	}


	bool BackendHazardMask::Equals(BackendHazardMask other) const
	{
		return Mask.Equals(other.Mask);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public override bool Equals(Object? obj)
	bool BackendHazardMask::Equals(std::any obj) const
	{
		const auto *m = std::any_cast<BackendHazardMask>(&obj); return m != nullptr && Equals(*m);
	}

	int BackendHazardMask::GetHashCode() const
	{
		return Mask.GetHashCode();
	}

	std::wstring BackendHazardMask::ToString() const
	{
		return std::format(L"BackendHazardMask({})", Mask.ToString());
	}
}
