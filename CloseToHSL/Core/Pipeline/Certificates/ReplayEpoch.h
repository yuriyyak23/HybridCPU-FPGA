#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	/// <summary>
	/// Checked representation of one issued loop-buffer replay epoch.
	/// Representation proves only the non-zero UInt64 shape. It does not grant
	/// loop-buffer ownership, admission, scheduler reuse, execution, completion,
	/// replay, retirement or publication authority.
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct ReplayEpoch
//	{
//		public const ulong MinValue = 1UL;
//		public const ulong MaxValue = ulong.MaxValue;
//
//		public ulong Value
//		{
//			get;
//		}
//
//		/// <summary>True only for the issued non-zero representation.</summary>
//		public bool IsIssued
//		{
//			get
//			{
//				return IsRepresentable(Value);
//			}
//		}
//
//		public ReplayEpoch(ulong value)
//		{
//			if (!IsRepresentable(value))
//			{
//				throw new ArgumentOutOfRangeException(nameof(value), value, "A replay epoch must be non-zero.");
//			}
//
//			Value = value;
//		}
//
//		public static bool IsRepresentable(ulong value)
//		{
//			return value >= MinValue;
//		}
//
//		public static ReplayEpoch Create(ulong value)
//		{
//			return new(value);
//		}
//
//		public static bool TryCreate(ulong value, out ReplayEpoch epoch)
//		{
//			if (IsRepresentable(value))
//			{
//				epoch = new ReplayEpoch(value);
//				return true;
//			}
//
//			epoch = default;
//			return false;
//		}
//
//		/// <summary>Reconstructs an issued epoch from its retained UInt64 form.</summary>
//		public static ReplayEpoch FromRawValue(ulong value)
//		{
//			return new(value);
//		}
//
//		/// <summary>Projects an issued value, or default absence, to raw UInt64.</summary>
//		public ulong ToRawValue()
//		{
//			return Value;
//		}
//
//		public override string ToString()
//		{
//			return IsIssued ? string.Format("replay-epoch{0}", Value) : "unissued";
//		}
//	}

}
