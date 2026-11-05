#pragma once

#include "MemoryCapability.h"
#include <optional>

namespace YAKSys_Hybrid_CPU::Core::Decoder
{

	/// <summary>
	/// Classification of a scheduler-visible memory-bank resolution attempt.
	/// This is representational state only; it grants no routing, admission,
	/// execution, completion or publication authority.
	/// </summary>
	enum class MemoryBankResolutionKind : unsigned char
	{
		/// <summary>
		/// The live memory topology required for resolution is unavailable.
		/// This zero-valued case also makes the default result fail closed.
		/// </summary>
		UnavailableTopology = 0,

		/// <summary>
		/// Resolution produced one representable scheduler-visible bank.
		/// </summary>
		Resolved = 1,

		/// <summary>
		/// Supplied geometry cannot represent the architectural bank topology.
		/// </summary>
		InvalidGeometry = 2
	};

	/// <summary>
	/// Three-way result for scheduler-visible bank resolution. Non-resolved
	/// results carry no <see cref="MemoryBankId"/>; bank zero remains a valid
	/// resolved identity and is never used as absence.
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MemoryBankResolution
//	{
//		private readonly System.Nullable<MemoryBankId> _bank;
//
//		private MemoryBankResolution(MemoryBankResolutionKind kind, System.Nullable<MemoryBankId> bank)
//		{
//			Kind = kind;
//			_bank = bank;
//		}
//
//		public MemoryBankResolutionKind Kind
//		{
//			get;
//		}
//
//		public System.Nullable<MemoryBankId> Bank
//		{
//			get
//			{
//				return Kind == MemoryBankResolutionKind.Resolved ? _bank : nullptr;
//			}
//		}
//
//		public bool IsResolved
//		{
//			get
//			{
//				return Kind == MemoryBankResolutionKind.Resolved;
//			}
//		}
//
//		public static MemoryBankResolution UnavailableTopology
//		{
//			get
//			{
//				return default;
//			}
//		}
//
//		public static MemoryBankResolution InvalidGeometry
//		{
//			get
//			{
//				return new(MemoryBankResolutionKind.InvalidGeometry, bank: nullptr);
//			}
//		}
//
//		public static MemoryBankResolution Resolved(MemoryBankId bank)
//		{
//			return new(MemoryBankResolutionKind.Resolved, bank);
//		}
//
//		public bool TryGetResolved(out MemoryBankId bank)
//		{
//			if (Kind == MemoryBankResolutionKind.Resolved && _bank.HasValue)
//			{
//				bank = _bank.Value;
//				return true;
//			}
//
//			bank = default;
//			return false;
//		}
//
//		public override string ToString()
//		{
//// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
////			return Kind switch
////		{
////			MemoryBankResolutionKind.Resolved => $"Resolved({_bank.GetValueOrDefault()})",
////			MemoryBankResolutionKind.UnavailableTopology => nameof(MemoryBankResolutionKind.UnavailableTopology),
////			MemoryBankResolutionKind.InvalidGeometry => nameof(MemoryBankResolutionKind.InvalidGeometry),
////			_ => throw new InvalidOperationException(
////				$"Unknown memory-bank resolution kind: {Kind}."), };
//		}
//	}

    struct MemoryBankResolution final
    {
        const MemoryBankResolutionKind Kind;
        const std::optional<MemoryBankId> Bank;
        MemoryBankResolution(MemoryBankResolutionKind kind, std::optional<MemoryBankId> bank = std::nullopt) : Kind(kind), Bank(bank) {}
        bool IsResolved() const noexcept { return Kind == MemoryBankResolutionKind::Resolved && Bank.has_value(); }
        bool TryGetResolved(MemoryBankId& bank) const noexcept { if (!IsResolved()) return false; bank = *Bank; return true; }
        static const MemoryBankResolution UnavailableTopology;
        static const MemoryBankResolution InvalidGeometry;
        static MemoryBankResolution Resolved(MemoryBankId bank) { return MemoryBankResolution(MemoryBankResolutionKind::Resolved, bank); }
    };
    inline const MemoryBankResolution MemoryBankResolution::UnavailableTopology{MemoryBankResolutionKind::UnavailableTopology};
    inline const MemoryBankResolution MemoryBankResolution::InvalidGeometry{MemoryBankResolutionKind::InvalidGeometry};}

