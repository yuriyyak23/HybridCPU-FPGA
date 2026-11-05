#pragma once

#include "MemoryCapability.h"
#include <memory>
#include <utility>
#include <vector>

namespace YAKSys_Hybrid_CPU::Core::Decoder
{
	class AdmissionRecord;

	/// <summary>Immutable Stage-A memory admission envelope.</summary>
	class MemoryAdmissionPolicy final
	{
	private:
		MemoryAdmissionPolicy(std::vector<MemoryBankId> allowedBanks,
			MemoryAccessDirection allowedDirections,
			std::vector<FrozenMemoryRange> allowedFootprint)
			: AllowedBanks(std::move(allowedBanks)), AllowedDirections(allowedDirections),
			  AllowedFootprint(std::move(allowedFootprint)) {}

	public:
		const std::vector<MemoryBankId> AllowedBanks;
		const MemoryAccessDirection AllowedDirections;
		const std::vector<FrozenMemoryRange> AllowedFootprint;

		static std::shared_ptr<MemoryAdmissionPolicy> Create(
			std::vector<MemoryBankId> allowedBanks,
			MemoryAccessDirection allowedDirections,
			std::vector<FrozenMemoryRange> allowedFootprint);
	};

	/// <summary>
	/// Immutable Stage-A memory admission envelope. It contains only static bank,
	/// direction and address-footprint legality. Dynamic pressure, timing, MSHR
	/// allocation and completion remain outside RF-06.4a.
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record MemoryAdmissionPolicy
//	{
//		private MemoryAdmissionPolicy(ImmutableArray<MemoryBankId> allowedBanks, MemoryAccessDirection allowedDirections, ImmutableArray<FrozenMemoryRange> allowedFootprint)
//		{
//			AllowedBanks = allowedBanks;
//			AllowedDirections = allowedDirections;
//			AllowedFootprint = allowedFootprint;
//		}
//
//		public ImmutableArray<MemoryBankId> AllowedBanks
//		{
//			get;
//		}
//		public MemoryAccessDirection AllowedDirections
//		{
//			get;
//		}
//		public ImmutableArray<FrozenMemoryRange> AllowedFootprint
//		{
//			get;
//		}
//
//		public static MemoryAdmissionPolicy Create(IEnumerable<MemoryBankId> allowedBanks, MemoryAccessDirection allowedDirections, IEnumerable<FrozenMemoryRange> allowedFootprint)
//		{
//			ArgumentNullException.ThrowIfNull(allowedBanks);
//			ArgumentNullException.ThrowIfNull(allowedFootprint);
//			if (allowedDirections == MemoryAccessDirection.None)
//			{
//				throw new ArgumentException("A memory admission policy must allow at least one direction.", nameof(allowedDirections));
//			}
//
//			MemoryBankId[] banks = allowedBanks.Distinct().OrderBy(bank => bank.Value).ToArray();
//			if (banks.Length == 0)
//			{
//				throw new ArgumentException("A memory admission policy must allow at least one bank.", nameof(allowedBanks));
//			}
//
//			ImmutableArray<FrozenMemoryRange> footprint = NormalizeFootprint(allowedFootprint);
//			if (footprint.IsDefaultOrEmpty)
//			{
//				throw new ArgumentException("A memory admission policy must declare at least one allowed range.", nameof(allowedFootprint));
//			}
//
//			return new MemoryAdmissionPolicy(ImmutableArray.Create(banks), allowedDirections, footprint);
//		}
//
//		private static ImmutableArray<FrozenMemoryRange> NormalizeFootprint(IEnumerable<FrozenMemoryRange> ranges)
//		{
//			FrozenMemoryRange[] ordered = ranges.OrderBy(range => range.Address).ThenBy(range => range.Length).ToArray();
//
//			for (int index = 1; index < ordered.Length; index++)
//			{
//				FrozenMemoryRange previous = ordered[index - 1];
//				FrozenMemoryRange current = ordered[index];
//				if (current.Address <= previous.LastAddress)
//				{
//					throw new ArgumentException("A memory admission policy cannot contain overlapping allowed ranges.", nameof(ranges));
//				}
//			}
//
//			return ImmutableArray.Create(ordered);
//		}
//	}

	enum class Rf06MemoryAdmissionRejectReason : unsigned char
	{
		None = 0,
		NotMemoryCapability = 1,
		BankNotAllowed = 2,
		DirectionNotAllowed = 3,
		FootprintOutsidePolicy = 4
	};

	class Rf06MemoryAdmissionResult final
	{
	private:
		Rf06MemoryAdmissionResult(bool isAdmitted, Rf06MemoryAdmissionRejectReason rejectReason,
			std::shared_ptr<AdmissionRecord> admission)
			: IsAdmitted(isAdmitted), RejectReason(rejectReason), Admission(std::move(admission)) {}

	public:
		const bool IsAdmitted;
		const Rf06MemoryAdmissionRejectReason RejectReason;
		const std::shared_ptr<AdmissionRecord> Admission;

		static std::shared_ptr<Rf06MemoryAdmissionResult> Accepted(std::shared_ptr<AdmissionRecord> admission);
		static std::shared_ptr<Rf06MemoryAdmissionResult> Rejected(
			std::shared_ptr<AdmissionRecord> admission, Rf06MemoryAdmissionRejectReason reason);
	};

	/// <summary>
	/// Result of the RF-06.4a static memory Stage-A check. It intentionally has no
	/// carrier, lane, operation ID, replay entry or mutable pressure state.
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal sealed record Rf06MemoryAdmissionResult(bool IsAdmitted, Rf06MemoryAdmissionRejectReason RejectReason, AdmissionRecord Admission)
//	{
//		internal static Rf06MemoryAdmissionResult Accepted(AdmissionRecord admission)
//		{
//			return new(true, Rf06MemoryAdmissionRejectReason.None, admission);
//		}
//
//		internal static Rf06MemoryAdmissionResult Rejected(AdmissionRecord admission, Rf06MemoryAdmissionRejectReason reason)
//		{
//			return new(false, reason, admission);
//		}
//	}

	/// <summary>
	/// RF-06.4a checked memory capability admission. This is a static Stage-A
	/// contract check; the existing scheduler remains responsible for dynamic bank
	/// pressure and the later physical-lane path.
	/// </summary>
	class Rf06MemoryCapabilityAdmission final
	{
	public:
		static std::shared_ptr<Rf06MemoryAdmissionResult> AdmitStageA(std::shared_ptr<AdmissionRecord> admission, std::shared_ptr<MemoryAdmissionPolicy> policy);

	private:
		static bool Contains(const FrozenMemoryRange &allowed, const FrozenMemoryRange &requested);
	};

}
