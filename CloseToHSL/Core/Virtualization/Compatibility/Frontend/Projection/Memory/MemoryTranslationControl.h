#pragma once

namespace YAKSys_Hybrid_CPU::Core
{
	// Value projection matching the frozen C# record's public data contract.
	struct MemoryTranslationControl
	{
		static constexpr unsigned long long EnableNptBit = 1ULL << 1;
		static constexpr unsigned long long EnableVpidBit = 1ULL << 5;
		static constexpr unsigned char WriteBackMemoryType = 6;

		bool NptEnabled = false;
		bool VpidEnabled = false;
		unsigned long long GuestCr3 = 0;
		unsigned long long NptRoot = 0;
		unsigned short Vmid = 0;
		unsigned short Vpid = 0;
		unsigned long long VmcsIdentity = 0;
		unsigned long long VmcsEpoch = 0;
		unsigned char DefaultMemoryType = 6;

		static MemoryTranslationControl Disabled() noexcept { return {}; }
	};

	/// <summary>
	/// Frozen read-only compatibility translation projection. It cannot produce
	/// memory-domain state, address-space identity, or invalidation epochs.
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MemoryTranslationControl(bool NptEnabled, bool VpidEnabled, ulong GuestCr3, ulong NptRoot, ushort Vmid, ushort Vpid, ulong VmcsIdentity, ulong VmcsEpoch, byte DefaultMemoryType)
//	{
//		public const ulong EnableNptBit = 1UL << 1;
//		public const ulong EnableVpidBit = 1UL << 5;
//		public const byte WriteBackMemoryType = 6;
//
//		public bool SecondStageTranslationEnabled
//		{
//			get
//			{
//				return NptEnabled;
//			}
//		}
//
//		public bool AddressSpaceTaggingEnabled
//		{
//			get
//			{
//				return VpidEnabled;
//			}
//		}
//
//		public ulong SecondStageRoot
//		{
//			get
//			{
//				return NptRoot;
//			}
//		}
//
//		public ushort AddressSpaceTag
//		{
//			get
//			{
//				return Vpid;
//			}
//		}
//
//		public ulong AddressSpaceRoot
//		{
//			get
//			{
//				return GuestCr3;
//			}
//		}
//
//		public ulong GuestAddressSpaceRoot
//		{
//			get
//			{
//				return GuestCr3;
//			}
//		}
//
//		public ulong CompatibilityProjectionIdentity
//		{
//			get
//			{
//				return VmcsIdentity;
//			}
//		}
//
//		public ulong AddressSpaceGeneration
//		{
//			get
//			{
//				return VmcsEpoch;
//			}
//		}
//
//		public bool HasCompatibilityProjectionIdentity
//		{
//			get
//			{
//				return CompatibilityProjectionIdentity != 0;
//			}
//		}
//
//		public bool IsReadOnlyCompatibilityProjection
//		{
//			get
//			{
//				return true;
//			}
//		}
//
//		public bool IsValid
//		{
//			get
//			{
//				return !SecondStageTranslationEnabled || ((GuestAddressSpaceRoot & 0xFFFUL) == 0 && (SecondStageRoot & 0xFFFUL) == 0 && (!AddressSpaceTaggingEnabled || AddressSpaceTag != 0));
//			}
//		}
//
//		public static MemoryTranslationControl Disabled
//		{
//			get;
//		} = new(NptEnabled: false, VpidEnabled: false, GuestCr3: 0, NptRoot: 0, Vmid: 0, Vpid: 0, VmcsIdentity: 0, VmcsEpoch: 0, DefaultMemoryType: WriteBackMemoryType);
//
//	}
}
