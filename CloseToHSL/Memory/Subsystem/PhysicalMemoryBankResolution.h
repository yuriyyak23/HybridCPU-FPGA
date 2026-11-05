#pragma once

namespace YAKSys_Hybrid_CPU::Memory
{

	/// <summary>
	/// Discriminant for a physical memory-bank resolution result. This is
	/// distinct from scheduler-visible memory-bank resolution.
	/// </summary>
	enum class PhysicalMemoryBankResolutionKind : unsigned char
	{
		Unavailable = 0,
		Resolved = 1
	};

	/// <summary>
	/// Exact reason that physical memory-bank resolution is unavailable.
	/// Unavailable results carry no bank index or geometry generation.
	/// </summary>
	enum class PhysicalMemoryBankUnavailableReason : unsigned char
	{
		NoPublishedGeometry = 0,
		InvalidBankCount = 1,
		InvalidBankWidth = 2,
		GenerationUnavailable = 3
	};

	/// <summary>
	/// Discriminated physical memory-bank resolution result. A resolved result
	/// contains exactly one checked binding; an unavailable result contains only
	/// one reason. Representation grants no legality, ownership, admission,
	/// request acceptance, execution, completion, cancellation, replay,
	/// retirement, store visibility or publication authority.
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct PhysicalMemoryBankResolution
//	{
//		private readonly System.Nullable<PhysicalMemoryBankBinding> _binding;
//		private readonly PhysicalMemoryBankUnavailableReason _unavailableReason;
//
//		private PhysicalMemoryBankResolution(PhysicalMemoryBankResolutionKind kind, System.Nullable<PhysicalMemoryBankBinding> binding, PhysicalMemoryBankUnavailableReason unavailableReason)
//		{
//			Kind = kind;
//			_binding = binding;
//			_unavailableReason = unavailableReason;
//		}
//
//		public PhysicalMemoryBankResolutionKind Kind
//		{
//			get;
//		}
//
//		public bool IsResolved
//		{
//			get
//			{
//				return Kind == PhysicalMemoryBankResolutionKind.Resolved;
//			}
//		}
//
//		public System.Nullable<PhysicalMemoryBankBinding> Binding
//		{
//			get
//			{
//				return IsResolved ? _binding : nullptr;
//			}
//		}
//
//		public System.Nullable<PhysicalMemoryBankUnavailableReason> UnavailableReason
//		{
//			get
//			{
//				return Kind == PhysicalMemoryBankResolutionKind.Unavailable ? _unavailableReason : nullptr;
//			}
//		}
//
//		/// <summary>
//		/// The default result fails closed as no published geometry.
//		/// </summary>
//		public static PhysicalMemoryBankResolution NoPublishedGeometry
//		{
//			get
//			{
//				return default;
//			}
//		}
//
//		public static PhysicalMemoryBankResolution Resolved(PhysicalMemoryBankBinding binding)
//		{
//			if (!binding.IsWellFormed)
//			{
//				throw new ArgumentOutOfRangeException(nameof(binding), binding, "A resolved physical memory-bank result requires a " + "well-formed binding.");
//			}
//
//			return new PhysicalMemoryBankResolution(PhysicalMemoryBankResolutionKind.Resolved, binding, default);
//		}
//
//		public static PhysicalMemoryBankResolution Unavailable(PhysicalMemoryBankUnavailableReason reason)
//		{
//			if (!IsRepresentable(reason))
//			{
//				throw new ArgumentOutOfRangeException(nameof(reason), reason, "Unknown physical memory-bank unavailable reason.");
//			}
//
//			return reason == PhysicalMemoryBankUnavailableReason.NoPublishedGeometry ? default : new PhysicalMemoryBankResolution(PhysicalMemoryBankResolutionKind.Unavailable, binding: nullptr, reason);
//		}
//
//		public static bool IsRepresentable(PhysicalMemoryBankUnavailableReason reason)
//		{
//			return reason is >= PhysicalMemoryBankUnavailableReason.NoPublishedGeometry @and <= PhysicalMemoryBankUnavailableReason.GenerationUnavailable;
//		}
//
//		public bool TryGetResolved(out PhysicalMemoryBankBinding binding)
//		{
//			if (IsResolved && _binding is { IsWellFormed: true } resolved)
//			{
//				binding = resolved;
//				return true;
//			}
//
//			binding = default;
//			return false;
//		}
//
//		public bool TryGetUnavailableReason(out PhysicalMemoryBankUnavailableReason reason)
//		{
//			if (Kind == PhysicalMemoryBankResolutionKind.Unavailable && IsRepresentable(_unavailableReason))
//			{
//				reason = _unavailableReason;
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
////			PhysicalMemoryBankResolutionKind.Unavailable
////				when IsRepresentable(_unavailableReason) => $"Unavailable({_unavailableReason})",
////			PhysicalMemoryBankResolutionKind.Resolved
////				when _binding is { IsWellFormed: true } =>
////				$"Resolved({_binding.Value})",
////			_ => throw new InvalidOperationException(
////				$"Malformed physical memory-bank resolution kind: {Kind}."),
////		};
//		}
//	}

}
