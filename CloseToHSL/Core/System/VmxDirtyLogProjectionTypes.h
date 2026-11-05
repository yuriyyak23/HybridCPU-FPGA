#pragma once

#include <vector>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class VmxDirtyWriteSource : unsigned char
	{
		Unknown = 0,
		Cpu = 1,
		Atomic = 2,
		Dma = 3,
		Lane6 = 4,
		Lane7 = 5,
		NptWriteProtect = 6
	};

	enum class VmxDirtyLogOverflowPolicy : unsigned char
	{
		FailClosed = 0
	};

	struct VmxDirtyLogConfiguration final
	{
		static constexpr unsigned int DefaultPageSize = 4096;
		static constexpr unsigned int DefaultMaxDirtyPages = 1U << 20;

		const bool Enabled;
		const unsigned long long GuestPhysicalBase;
		const unsigned long long GuestPhysicalLimitExclusive;
		const unsigned int PageSize;
		const unsigned int MaxDirtyPages;
		const VmxDirtyLogOverflowPolicy OverflowPolicy;

		VmxDirtyLogConfiguration(bool enabled, unsigned long long guestPhysicalBase,
			unsigned long long guestPhysicalLimitExclusive, unsigned int pageSize,
			unsigned int maxDirtyPages, VmxDirtyLogOverflowPolicy overflowPolicy)
			: Enabled(enabled), GuestPhysicalBase(guestPhysicalBase), GuestPhysicalLimitExclusive(guestPhysicalLimitExclusive),
			  PageSize(pageSize), MaxDirtyPages(maxDirtyPages), OverflowPolicy(overflowPolicy) {}

		static VmxDirtyLogConfiguration Disabled()
		{
			return {false, 0, 0, DefaultPageSize, DefaultMaxDirtyPages, VmxDirtyLogOverflowPolicy::FailClosed};
		}

		static VmxDirtyLogConfiguration EnabledForRange(unsigned long long guestPhysicalBase,
			unsigned long long guestPhysicalLimitExclusive, unsigned int pageSize = DefaultPageSize,
			unsigned int maxDirtyPages = DefaultMaxDirtyPages)
		{
			return {true, guestPhysicalBase, guestPhysicalLimitExclusive,
				pageSize == 0 ? DefaultPageSize : pageSize,
				maxDirtyPages == 0 ? DefaultMaxDirtyPages : maxDirtyPages,
				VmxDirtyLogOverflowPolicy::FailClosed};
		}
	};

	struct VmxDirtyLogStatus final
	{
		const bool Enabled;
		const unsigned int PageSize;
		const unsigned long long Generation;
		const unsigned long long WriteSequence;
		const bool Overflowed;
		const int DirtyPageCount;
		const unsigned long long CpuWriteCount;
		const unsigned long long AtomicWriteCount;
		const unsigned long long DmaWriteCount;
		const unsigned long long Lane6WriteCount;
		const unsigned long long Lane7WriteCount;
		const unsigned long long NptWriteProtectCount;
		const unsigned long long DroppedWriteCount;

		VmxDirtyLogStatus(bool enabled, unsigned int pageSize, unsigned long long generation,
			unsigned long long writeSequence, bool overflowed, int dirtyPageCount,
			unsigned long long cpuWriteCount, unsigned long long atomicWriteCount,
			unsigned long long dmaWriteCount, unsigned long long lane6WriteCount,
			unsigned long long lane7WriteCount, unsigned long long nptWriteProtectCount,
			unsigned long long droppedWriteCount)
			: Enabled(enabled), PageSize(pageSize), Generation(generation), WriteSequence(writeSequence),
			  Overflowed(overflowed), DirtyPageCount(dirtyPageCount), CpuWriteCount(cpuWriteCount),
			  AtomicWriteCount(atomicWriteCount), DmaWriteCount(dmaWriteCount), Lane6WriteCount(lane6WriteCount),
			  Lane7WriteCount(lane7WriteCount), NptWriteProtectCount(nptWriteProtectCount), DroppedWriteCount(droppedWriteCount) {}

		unsigned long long getTotalAcceptedWrites() const
		{
			return CpuWriteCount + AtomicWriteCount + DmaWriteCount + Lane6WriteCount + Lane7WriteCount + NptWriteProtectCount;
		}
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct VmxDirtyLogConfiguration(bool Enabled, ulong GuestPhysicalBase, ulong GuestPhysicalLimitExclusive, uint PageSize, uint MaxDirtyPages, VmxDirtyLogOverflowPolicy OverflowPolicy)
//	{
//		public const uint DefaultPageSize = 4096;
//		public const uint DefaultMaxDirtyPages = 1 << 20;
//
//		public static VmxDirtyLogConfiguration Disabled
//		{
//			get;
//		} = new(false, 0, 0, DefaultPageSize, DefaultMaxDirtyPages, VmxDirtyLogOverflowPolicy.FailClosed);
//
//		public static VmxDirtyLogConfiguration EnabledForRange(ulong guestPhysicalBase, ulong guestPhysicalLimitExclusive, uint pageSize = DefaultPageSize, uint maxDirtyPages = DefaultMaxDirtyPages)
//		{
//			return new(true, guestPhysicalBase, guestPhysicalLimitExclusive, pageSize == 0 ? DefaultPageSize : pageSize, maxDirtyPages == 0 ? DefaultMaxDirtyPages : maxDirtyPages, VmxDirtyLogOverflowPolicy.FailClosed);
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct VmxDirtyLogStatus(bool Enabled, uint PageSize, ulong Generation, ulong WriteSequence, bool Overflowed, int DirtyPageCount, ulong CpuWriteCount, ulong AtomicWriteCount, ulong DmaWriteCount, ulong Lane6WriteCount, ulong Lane7WriteCount, ulong NptWriteProtectCount, ulong DroppedWriteCount)
//	{
//		public ulong TotalAcceptedWrites
//		{
//			get
//			{
//				return CpuWriteCount + AtomicWriteCount + DmaWriteCount + Lane6WriteCount + Lane7WriteCount + NptWriteProtectCount;
//			}
//		}
//	}

	class VmxDirtyLogSnapshot final : public std::enable_shared_from_this<VmxDirtyLogSnapshot>
	{
	private:
		const bool Enabled;
		const unsigned int PageSize;
		const unsigned long long GuestPhysicalBase;
		const unsigned long long GuestPhysicalLimitExclusive;
		const unsigned long long Generation;
		const unsigned long long WriteSequence;
		const bool Overflowed;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly IReadOnlyList<ulong> DirtyPageIndices;
		const std::vector<unsigned long long> DirtyPageIndices;

	public:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public VmxDirtyLogSnapshot(bool enabled, uint pageSize, ulong guestPhysicalBase, ulong guestPhysicalLimitExclusive, ulong generation, ulong writeSequence, bool overflowed, IReadOnlyList<ulong> dirtyPageIndices)
		VmxDirtyLogSnapshot(bool enabled, unsigned int pageSize, unsigned long long guestPhysicalBase, unsigned long long guestPhysicalLimitExclusive, unsigned long long generation, unsigned long long writeSequence, bool overflowed, const std::vector<unsigned long long> &dirtyPageIndices);

		bool getEnabled() const;

		unsigned int getPageSize() const;

		unsigned long long getGuestPhysicalBase() const;

		unsigned long long getGuestPhysicalLimitExclusive() const;

		unsigned long long getGeneration() const;

		unsigned long long getWriteSequence() const;

		bool getOverflowed() const;

		const std::vector<unsigned long long> getDirtyPageIndices() const;

		bool ContainsGuestPhysicalAddress(unsigned long long address);
	};

}
