#pragma once
#include <memory>
#include <optional>

namespace YAKSys_Hybrid_CPU::Memory
{

	/// <summary>
	/// Discriminant for an authoritative physical memory-bank geometry update
	/// result.
	/// </summary>
	enum class MemoryBankGeometryUpdateResultKind : unsigned char
	{
		Rejected = 0,
		Applied = 1
	};

	/// <summary>
	/// Exact reason that a physical memory-bank geometry update was rejected.
	/// </summary>
	enum class MemoryBankGeometryUpdateRejectReason : unsigned char
	{
		InvalidBankCount = 0,
		InvalidBankWidth = 1,
		Busy = 2,
		GenerationExhausted = 3,
		PlatformRejected = 4
	};

	/// <summary>
	/// Discriminated result for an authoritative physical memory-bank geometry
	/// update. This representation does not validate or publish geometry, allocate
	/// a generation, prove quiescence, prepare owner state, or grant lifecycle
	/// authority.
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MemoryBankGeometryUpdateResult
//	{
//		private readonly MemoryBankGeometryUpdateRejectReason _rejectReason;
//
//		private MemoryBankGeometryUpdateResult(MemoryBankGeometryUpdateResultKind kind, MemoryBankGeometryUpdateRejectReason rejectReason)
//		{
//			Kind = kind;
//			_rejectReason = rejectReason;
//		}
//
//		public MemoryBankGeometryUpdateResultKind Kind
//		{
//			get;
//		}
//
//		public bool IsApplied
//		{
//			get
//			{
//				return Kind == MemoryBankGeometryUpdateResultKind.Applied;
//			}
//		}
//
//		public System.Nullable<MemoryBankGeometryUpdateRejectReason> RejectReason
//		{
//			get
//			{
//				return Kind == MemoryBankGeometryUpdateResultKind.Rejected ? _rejectReason : nullptr;
//			}
//		}
//
//		/// <summary>
//		/// The default result fails closed as an invalid bank-count rejection.
//		/// </summary>
//		public static MemoryBankGeometryUpdateResult InvalidBankCount
//		{
//			get
//			{
//				return default;
//			}
//		}
//
//		public static MemoryBankGeometryUpdateResult Applied
//		{
//			get
//			{
//				return new(MemoryBankGeometryUpdateResultKind.Applied, default);
//			}
//		}
//
//		public static MemoryBankGeometryUpdateResult Rejected(MemoryBankGeometryUpdateRejectReason reason)
//		{
//			if (!IsRepresentable(reason))
//			{
//				throw new ArgumentOutOfRangeException(nameof(reason), reason, "Unknown memory-bank geometry update rejection reason.");
//			}
//
//			return reason == MemoryBankGeometryUpdateRejectReason.InvalidBankCount ? default : new MemoryBankGeometryUpdateResult(MemoryBankGeometryUpdateResultKind.Rejected, reason);
//		}
//
//		public static bool IsRepresentable(MemoryBankGeometryUpdateRejectReason reason)
//		{
//			return reason is >= MemoryBankGeometryUpdateRejectReason.InvalidBankCount @and <= MemoryBankGeometryUpdateRejectReason.PlatformRejected;
//		}
//
//		public bool TryGetRejectReason(out MemoryBankGeometryUpdateRejectReason reason)
//		{
//			if (Kind == MemoryBankGeometryUpdateResultKind.Rejected && IsRepresentable(_rejectReason))
//			{
//				reason = _rejectReason;
//				return true;
//			}
//
//			reason = default;
//			return false;
//		}
//
//		public override string ToString()
//		{
//// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
////			return Kind switch
////		{
////			MemoryBankGeometryUpdateResultKind.Rejected
////				when IsRepresentable(_rejectReason) => $"Rejected({_rejectReason})",
////			MemoryBankGeometryUpdateResultKind.Applied => "Applied",
////			_ => throw new InvalidOperationException(
////				$"Malformed memory-bank geometry update result kind: {Kind}."),
////		};
//		}
//	}

}

namespace YAKSys_Hybrid_CPU::Memory
{
struct MemoryBankGeometryUpdateResult final
{
    const MemoryBankGeometryUpdateResultKind Kind;
    const std::optional<MemoryBankGeometryUpdateRejectReason> RejectReason;
    const bool IsApplied;
    MemoryBankGeometryUpdateResult(MemoryBankGeometryUpdateResultKind kind, std::optional<MemoryBankGeometryUpdateRejectReason> reason)
        : Kind(kind), RejectReason(reason), IsApplied(kind == MemoryBankGeometryUpdateResultKind::Applied) {}
    static std::shared_ptr<MemoryBankGeometryUpdateResult> Rejected(MemoryBankGeometryUpdateRejectReason reason)
    { return std::make_shared<MemoryBankGeometryUpdateResult>(MemoryBankGeometryUpdateResultKind::Rejected, reason); }
    static const std::shared_ptr<MemoryBankGeometryUpdateResult> Applied;
};
inline const std::shared_ptr<MemoryBankGeometryUpdateResult> MemoryBankGeometryUpdateResult::Applied =
    std::make_shared<MemoryBankGeometryUpdateResult>(MemoryBankGeometryUpdateResultKind::Applied, std::nullopt);
}