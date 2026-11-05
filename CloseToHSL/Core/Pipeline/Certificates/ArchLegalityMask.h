#pragma once

#include "SafetyMask128.h"
#include <string>
#include <any>
#include <format>

namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// Architectural legality mask — encodes ISA-level hazards detectable from
	/// architectural register IDs alone (RAW, WAR, WAW via arch-reg groups).
	///
	/// Blueprint §7 / Checklist: "Развести ArchLegalityMask и BackendHazardMask."
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct ArchLegalityMask : IEquatable<ArchLegalityMask>
	class ArchLegalityMask
	{
	public:
		SafetyMask128 Mask;

		explicit ArchLegalityMask(SafetyMask128 mask);
		ArchLegalityMask(unsigned long long low, unsigned long long high = 0);

		bool isZero() const;

		bool ConflictsWith(ArchLegalityMask other) const;

		ArchLegalityMask operator | (ArchLegalityMask b);

		ArchLegalityMask operator &(ArchLegalityMask b);

		operator SafetyMask128();

		bool Equals(ArchLegalityMask other) const ;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public override bool Equals(Object? obj)
		bool Equals(std::any obj) const;
		int GetHashCode() const;
		std::wstring ToString() const;

		static const ArchLegalityMask Zero;

		ArchLegalityMask() : Mask(YAKSys_Hybrid_CPU::Core::SafetyMask128())
		{
		}
	};

	/// <summary>
	/// Backend hazard mask — encodes physical-register bank and execution-unit
	/// contention hazards at the microarchitecture level.
	///
	/// Blueprint §7 / Checklist: "Развести ArchLegalityMask и BackendHazardMask."
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct BackendHazardMask : IEquatable<BackendHazardMask>
	class BackendHazardMask
	{
	public:
		SafetyMask128 Mask;

		explicit BackendHazardMask(SafetyMask128 mask);
		BackendHazardMask(unsigned long long low, unsigned long long high = 0);

		bool isZero() const;

		bool ConflictsWith(BackendHazardMask other) const;

		BackendHazardMask operator | (BackendHazardMask b);

		BackendHazardMask operator &(BackendHazardMask b);

		operator SafetyMask128();

		bool Equals(BackendHazardMask other) const ;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public override bool Equals(Object? obj)
		bool Equals(std::any obj) const;
		int GetHashCode() const;
		std::wstring ToString() const;

		static const BackendHazardMask Zero;

		BackendHazardMask() : Mask(YAKSys_Hybrid_CPU::Core::SafetyMask128())
		{
		}
	};
inline const ArchLegalityMask ArchLegalityMask::Zero = ArchLegalityMask(SafetyMask128::Zero);
inline const BackendHazardMask BackendHazardMask::Zero = BackendHazardMask(SafetyMask128::Zero);
}
